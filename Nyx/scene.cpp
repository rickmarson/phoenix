/*
 
 Scene
 
*/

#include "scene.h"
#include "UI/ui_manager.h"
#include "label.h"
#include "Cache/resource_cache.h"
#include "camera_tp.h"
#include "camera_fp.h"
#include "particle_node.h"
#include "Physics/rigid_body.h"
#include "Math/vector3.h"
#include "Math/vector4.h"
#include "Math/matrix3x3.h"

#include "json.hpp"

using nlohmann::json;

namespace NYX {
    
static const std::string SCENE_EXTENSION = ".json";
static const std::string SCRIPT_EXTENSION = ".lua";
static const std::string KERNEL_EXTENSION = ".cl";
static const std::string OBJECT_EXTENSION = ".obj";
static const std::string MATERIAL_EXTENSION = ".mtl";

// Utility JSON Parsers
class CameraParser
{
public:
    void operator() ( const json& camera_source, SceneNodePtr parent, IRenderer* renderer, float aspect_ratio );
    
    std::string mTargetName;
    CameraNode* mCamera = nullptr;
    SceneNodePtr mCnode;
};

class LightParser
{
public:
    void operator() ( const json& light_source, SceneNodePtr parent, IRenderer* renderer );
};
    
class ObjectParser
{
public:
    void operator() ( const json& object_source, SceneNodePtr parent, IRenderer* renderer, float aspect_ratio );
    
    std::list<CameraNode*> mCameras;
    std::map<SceneNodePtr, string> mTargets;
    std::list<PhysicsBodyPtr> mRigidBodies;
};
    
class SkyBoxParser
{
public:
    void operator() ( const json& skybox_source, RootNodePtr root_node, IRenderer* renderer );
};
    
class PhysicsParser
{
public:
    void operator() ( const json& physics_source, PhysicsEnginePtr physics_engine );
};

class RigidBodyParser
{
public:
    void operator() ( const json& rb_source, SceneNodePtr parent, IRenderer* renderer, bool has_parent );
    
    PhysicsBodyPtr rigidbody;
};
    
class ParticleEmitterParser
{
public:
    void operator() ( const json& fx_source, SceneNodePtr parent, IRenderer* renderer );
};
  
// Scene implementation
Scene::Scene(ApplicationPtr application)
{
	mApplication = application;
	mEventMng = EventManager::GetInstance();
	//TODO add a factory for physics engines
	pPhysicsEngine = PhysicsEnginePtr( new DefaultPhysicsEngine() );
	pPhysicsEngine->InitPhysics(PHYS_RIGID_ONLY);
}

Scene::~Scene()
{
	pPhysicsEngine.reset();
}

void Scene::Update( void )
{
    static uint refreshNumber = 0;
    uint deltaT = 0;
    bool updateAnimation;
    
    mEventMng->ProcessEventQueue();
    
    mApplication->mCurTime = SDL_GetTicks();
    deltaT = mApplication->mCurTime - mApplication->mLoopStartTime;
    
    updateAnimation = (refreshNumber == 0 || (deltaT / mApplication->GetAnimationRate()) != refreshNumber);
    
    refreshNumber = (deltaT / mApplication->GetAnimationRate());
    
    if ( pPhysicsEngine )
    {
        //update physics, for the moment with the same frequency
        pPhysicsEngine->StepSimulation( (float)deltaT / 1000.0 );
    }
    
    mApplication->GetWindowPtr()->RenderFrame(updateAnimation);
}

void Scene::HandleUIEvent(UIEvent& event)
{
    switch ( event.id )
    {
        case UI_EVENT_ID::UI_KEY_PRESSED:
            if ( ((UIKeyPressedEvent&)event).key == SDLK_ESCAPE )
            {
                mApplication->bIsRunning = false;
            }
            break;
        default:
            break;
    }

}
    
bool Scene::SetupScene( std::string scene_name )
{
    char* scene_raw = ResourceCache::GetInstance()->RequestSourceCode(scene_name + SCENE_EXTENSION);
    mName = scene_name;
    
    // basic initialisation
    mWindow = mApplication->GetWindowPtr();
    
    WindowStatePtr scene( new WindowState(mWindow.get(), scene_name, this) );
    
    RootNodePtr root_node = scene->GetRootNode();
    root_node->SetAnimationRefreshRate(mApplication->GetAnimationRate());
    
    mRenderer = mWindow->GetRenderer();
    
    // read xml
    auto json_source = json::parse(scene_raw);
    
    
    if (json_source.empty())
    {
        //log message
        return false;
    }
    
    if ( false == (bool)(*(json_source.find("load"))) )
    {
        return false;
    }
    
    for ( auto iterator = json_source.begin(); iterator != json_source.end(); ++iterator )
    {
       if ( iterator.key() == "show title" )
       {
           scene->mShowTitle = iterator.value();
           if ( scene->mShowTitle )
           {
               scene->mTitle = json_source["title"].get<std::string>();
               std::string title = scene->mTitle.empty() ? scene_name : scene->mTitle;
               WidgetPtr title_label = Label::Create(mRenderer);
               ((Label*)title_label.get())->SetPosition( 750, 20 );
               ((Label*)title_label.get())->SetText( title, scene->mUIManager->GetDefaultFont(), 20 );
               scene->mUIManager->AddWidget(title_label);
           }
       }
       else if ( iterator.key() == "show first" )
       {
           scene->SetActive(iterator.value());
       }
       else if ( iterator.key() == "camera" )
       {
           CameraParser parser;
           parser(*iterator, root_node, mRenderer, mWindow->GetAspectRatio());
           if ( !parser.mTargetName.empty() )
           {
               targets[parser.mCnode] = parser.mTargetName;
           }
           
           mCameras.push_back(parser.mCamera);
       }
       else if ( iterator.key() == "light" )
       {
           LightParser parser;
           parser(*iterator, root_node, mRenderer);
       }
       else if ( iterator.key() == "object" )
       {
           ObjectParser parser;
           parser(*iterator, root_node, mRenderer, mWindow->GetAspectRatio());
           
           for ( auto rigid_body : parser.mRigidBodies )
           {
               pPhysicsEngine->AddBody(rigid_body);
           }
           
           for ( auto target_pair : parser.mTargets )
           {
               targets[target_pair.first] = target_pair.second;
           }
           
           for ( auto camera : parser.mCameras )
           {
               mCameras.push_back(camera);
           }
       }
       else if ( iterator.key() == "skybox" )
       {
           SkyBoxParser parser;
           parser(*iterator, root_node, mRenderer);
       }
       else if ( iterator.key() == "physics" )
       {
           PhysicsParser parser;
           parser(*iterator, pPhysicsEngine);
       }
       else if ( iterator.key() == "rigid body" )
       {
           RigidBodyParser parser;
           parser(*iterator, root_node, mRenderer, false);
           pPhysicsEngine->AddBody(parser.rigidbody);
       }
       else if ( iterator.key() == "particle emitter" )
       {
           ParticleEmitterParser parser;
           parser(*iterator, root_node, mRenderer);
       }
    }
    
    // Resolve targets
    // TODO: this only looks for top level nodes. 
    if ( !targets.empty() )
    {
        for ( auto node_iter = targets.begin(); node_iter != targets.end(); node_iter++ )
        {
            SceneNodePtr node = (*node_iter).first;
            string target = (*node_iter).second;
            
            SceneNodePtr target_node = root_node->GetChildNodeByName(target);
            
            if ( target_node )
            {
                node->SetTarget( target_node.get() );
            }
                
        }
    }
    
    // set up active camera
    for ( auto camera = mCameras.begin(); camera != mCameras.end(); camera++ )
    {
        root_node->AddCamera( *camera );
        
        if ( (*camera)->IsActive() )
        {
            root_node->mProjectionStack.PushMatrix( (*camera)->mFrustum.GetProjectionMatrix()); //need to make sure of where the proj matrix gets added to the stack!
            //it's ok here because it's added only once. In general, it should be the only one in the stack (except in very special cases), so the first matrix
            //should be updated, rather then a new one being pushed on top of it (very wrong!!!!)
        }
    }
    
    mWindow->AddWindowState(scene);
    
    return true;
}

void CameraParser::operator()( const json &camera_source, SceneNodePtr parent, IRenderer* renderer, float aspect_ratio )
{
    std::string name = *(camera_source.find("name"));
    std::string type = *(camera_source.find("type"));
    
    if ( type == "first person" )
    {
        mCnode = SceneNodePtr( new CameraFP(dynamic_cast<SceneNode*>(parent.get()), renderer, name));
        mCamera = dynamic_cast<CameraNode*>(mCnode.get());
    }
    else if ( type == "third person" )
    {
        mCnode = SceneNodePtr( new CameraTP(dynamic_cast<SceneNode*>(parent.get()), renderer, name));
        mCamera = dynamic_cast<CameraNode*>(mCnode.get());
    }
    
    for ( auto iterator = camera_source.begin(); iterator != camera_source.end(); ++iterator )
    {
        if ( iterator.key() == "active" )
        {
            mCamera->SetActive(iterator.value());
        }
        else if ( iterator.key() == "position" )
        {
            auto array = iterator.value();
            mCamera->SetRelativePosition( { array[0], array[1], array[2]  } );
        }
        else if ( iterator.key() == "look" )
        {
            auto array = iterator.value();
            Vector3 look_at(array[0], array[1], array[2]);
            mCamera->LookAt( look_at );
        }
        else if ( iterator.key() == "up" )
        {
            auto array = iterator.value();
            Vector3 up(array[0], array[1], array[2]);
            mCamera->UpVector( up );
        }
        else if ( iterator.key() == "attitude" )
        {
            auto array = iterator.value();
            
            Matrix3x3 rotx, roty, rotz;
            rotx.LoadRotX(array[0]);
            roty.LoadRotY(array[1]);
            rotz.LoadRotZ(array[2]);
            
            Matrix3x3 rotation = rotz * roty * rotx;
            mCamera->SetLocalAttitude(rotation);
        }
        else if ( iterator.key() == "script" )
        {
            auto script_source = iterator.value();
            
            string file_name = *(script_source.find("file name"));
            string func_name = *(script_source.find("function name"));
            file_name += SCRIPT_EXTENSION;
            mCamera->ScriptAnimation(file_name, func_name);
        }
        else if ( iterator.key() == "perspective" )
        {
            auto script_source = iterator.value();
            
            float clip_near = *(script_source.find("clip near"));
            float clip_far = *(script_source.find("clip far"));
            float fov = *(script_source.find("fov"));
            mCamera->mFrustum.SetPerspective(clip_near, clip_far, fov, aspect_ratio);
        }
        else if ( iterator.key() == "target" )
        {
            // targets need to be resolved when all the objects have been set up
            mTargetName = iterator.value().get<std::string>();
        }
    }
    
    parent->AddChildNode(mCnode);
}

void LightParser::operator()( const json &light_source, SceneNodePtr parent, IRenderer* renderer )
{
    string name = *(light_source.find("name"));
    
    SceneNodePtr lnode(new LightNode(dynamic_cast<SceneNode*>(parent.get()), renderer, name));
    LightNode* light = dynamic_cast<LightNode*>(lnode.get());
    

    for ( auto iterator = light_source.begin(); iterator != light_source.end(); ++iterator )
    {
        if ( iterator.key() == "active" )
        {
            light->SetActive(iterator.value());
        }
        else if ( iterator.key() == "directional" )
        {
            light->SetDirectional(iterator.value());
        }
        else if ( iterator.key() == "position" )
        {
            auto array = iterator.value();
            light->SetRelativePosition( { array[0], array[1], array[2]  } );
        }
        else if ( iterator.key() == "attitude" )
        {
            auto array = iterator.value();
            
            Matrix3x3 rotx, roty, rotz;
            rotx.LoadRotX(array[0]);
            roty.LoadRotY(array[1]);
            rotz.LoadRotZ(array[2]);
            
            Matrix3x3 rotation = rotz * roty * rotx;
            light->SetLocalAttitude(rotation);
        }
        else if ( iterator.key() == "diffuse" )
        {
            auto array = iterator.value();
            light->SetDiffuseColor( Vector4(array[0], array[1], array[2], array[3]).GetComponents() );
        }
        else if ( iterator.key() == "ambient" )
        {
            auto array = iterator.value();
            light->SetAmbientColor( Vector4(array[0], array[1], array[2], array[3]).GetComponents() );
        }
        else if ( iterator.key() == "specular" )
        {
            auto array = iterator.value();
            light->SetSpecularColor( Vector4(array[0], array[1], array[2], array[3]).GetComponents() );
        }
    }
    
    parent->AddChildNode(lnode);
}
    

void SkyBoxParser::operator()( const json &skybox_source, RootNodePtr root_node, NYX::IRenderer *renderer )
{
    string name, shader_name, texture_name;
    float scale = 1.0;
    
    for ( auto iterator = skybox_source.begin(); iterator != skybox_source.end(); ++iterator )
    {
        if ( iterator.key() == "cube texture" )
        {
            texture_name = iterator.value().get<std::string>();
        }
        else if ( iterator.key() == "shader name" )
        {
            shader_name = iterator.value().get<std::string>();
        }
        else if ( iterator.key() == "scale" )
        {
            scale = iterator.value().get<float>();
        }
    }
    
    root_node->AddSkyBox(name, texture_name, shader_name, scale);
}

void PhysicsParser::operator()( const json &physics_source, PhysicsEnginePtr physics_engine )
{
    for ( auto iterator = physics_source.begin(); iterator != physics_source.end(); ++iterator )
    {
        if ( iterator.key() == "gravity" )
        {
            auto array = iterator.value();
            physics_engine->SetGravity( { array[0], array[1], array[2] } );
        }
    }
}
    
void RigidBodyParser::operator()( const json &rb_source, SceneNodePtr parent, NYX::IRenderer *renderer, bool has_parent )
{
    if ( has_parent )
    {
        rigidbody = PhysicsBodyPtr( new RigidBody(parent->GetName(), parent->GetID()) );
        rigidbody->SetInitialState(parent->GetRelativePosition(), parent->GetLocalAttitude());
    }
    else
    {
        string name = *(rb_source.find("name"));
        rigidbody = PhysicsBodyPtr( new RigidBody(name, 9999) );
    }
    
    Vector3 position, inertia;
    Matrix3x3 rotation;
    float mass = 0;
    
    for ( auto iterator = rb_source.begin(); iterator != rb_source.end(); ++iterator )
    {
        if ( iterator.key() == "position" && !has_parent )
        {
            auto array = iterator.value();
            position = { array[0], array[1], array[2] };
        }
        else if ( iterator.key() == "attitude" && !has_parent )
        {
            auto array = iterator.value();
            
            Matrix3x3 rotx, roty, rotz;
            rotx.LoadRotX(array[0]);
            roty.LoadRotY(array[1]);
            rotz.LoadRotZ(array[2]);
            
            rotation = rotz * roty * rotx;
        }
        else if ( iterator.key() == "shape" )
        {
            auto shape_source = iterator.value();
            string shape_name = *(shape_source.find("name"));
            if ( shape_name == "sphere" )
            {
                float radius = *(shape_source.find("radius"));
                rigidbody->SetCollisionShape(CS_SPHERE, &radius);
            }
            else if ( shape_name == "box" )
            {
                auto array = *(shape_source.find("dimensions"));
                rigidbody->SetCollisionShape(CS_BOX, Vector3(array[0], array[1], array[2]).GetComponents());
            }
            
        }
        else if ( iterator.key() == "mass" )
        {
            mass = iterator.value();
        }
        else if ( iterator.key() == "inertia" )
        {
            auto array = iterator.value();
            inertia = {array[0], array[1], array[2]};
        }
        else if ( iterator.key() == "initial velocity" )
        {
            auto array = iterator.value();
            rigidbody->SetVelocity( {array[0], array[1], array[2]} );
        }
        else if ( iterator.key() == "friction" )
        {
            rigidbody->SetFriction( iterator.value() );
        }
    }
    
    if (!has_parent)
    {
        rigidbody->SetInitialState(position, rotation);
    }
    
    rigidbody->SetMassAndInertia(mass, inertia);
    
    rigidbody->Finilize();
}

void ParticleEmitterParser::operator()(const json &fx_source, SceneNodePtr parent, NYX::IRenderer *renderer)
{
    string name = *(fx_source.find("name"));
 
    SceneNodePtr fxnode(new ParticleNode(dynamic_cast<SceneNode*>(parent.get()), renderer, name));
    ParticleNode* fx_node = dynamic_cast<ParticleNode*>(fxnode.get());
    
    string program_file, kernel, material_name;
    int particle_count = 0;
    
    for ( auto iterator = fx_source.begin(); iterator != fx_source.end(); ++iterator )
    {
        if ( iterator.key() == "position" )
        {
            auto array = iterator.value();
            fx_node->SetRelativePosition( {array[0], array[1], array[2]} );
        }
        else if ( iterator.key() == "attitude" )
        {
            auto array = iterator.value();
            
            Matrix3x3 rotx, roty, rotz;
            rotx.LoadRotX(array[0]);
            roty.LoadRotY(array[1]);
            rotz.LoadRotZ(array[2]);
        
            Matrix3x3 rotation = rotz * roty * rotx;
            fx_node->SetLocalAttitude(rotation);
        }
        else if ( iterator.key() == "particle count" )
        {
            particle_count = iterator.value();
        }
        else if ( iterator.key() == "script" )
        {
            auto script_source = iterator.value();
            
            string file_name = *(script_source.find("file name"));
            file_name += SCRIPT_EXTENSION;
            
            string funcName = *(script_source.find("function name"));
            
            fx_node->UseScriptParticleGeneration(file_name, funcName);
        }
        else if ( iterator.key() == "kernel" )
        {
            auto kernel_source = iterator.value();

            program_file = kernel_source["file name"].get<std::string>();
            program_file += KERNEL_EXTENSION;
            
            kernel = kernel_source["kernel name"].get<std::string>();
        }
        else if ( iterator.key() == "material" )
        {
            material_name = iterator.value().get<std::string>();
            material_name += MATERIAL_EXTENSION;
        }
        else if ( iterator.key() == "light" )
        {
            LightParser parser;
            parser( iterator.value(), fxnode, renderer );
        }
    }
    
    fx_node->InitializeFX(program_file, kernel, particle_count, material_name);
    
    parent->AddChildNode(fxnode);
}
    
void ObjectParser::operator()( const json &object_source, SceneNodePtr parent, NYX::IRenderer *renderer, float aspect_ratio )
{
    string name = *(object_source.find("name"));
  
    SceneNodePtr mnode(new ModelNode(dynamic_cast<SceneNode*>(parent.get()), renderer, name));
    ModelNode* model_node = dynamic_cast<ModelNode*>(mnode.get());
    
    for ( auto iterator = object_source.begin(); iterator != object_source.end(); ++iterator )
    {
        if ( iterator.key() == "position" )
        {
            auto array = iterator.value();
            model_node->SetRelativePosition( {array[0], array[1], array[2]} );
        }
        else if ( iterator.key() == "model" )
        {
            string model_name = iterator.value();
            model_name += OBJECT_EXTENSION;
            model_node->AssignModel(model_name);
        }
        else if ( iterator.key() == "scale" )
        {
            model_node->SetScaleFactor( iterator.value() );
        }
        else if ( iterator.key() == "attitude" )
        {
            auto array = iterator.value();
            
            Matrix3x3 rotx, roty, rotz;
            rotx.LoadRotX(array[0]);
            roty.LoadRotY(array[1]);
            rotz.LoadRotZ(array[2]);
            
            Matrix3x3 rotation = rotz * roty * rotx;
            model_node->SetLocalAttitude(rotation);
        }
        else if ( iterator.key() == "rigid body" )
        {
            RigidBodyParser parser;
            parser(iterator.value(), mnode, renderer, true);
            mRigidBodies.push_back(parser.rigidbody);
        }
        else if ( iterator.key() == "camera" )
        {
            CameraParser parser;
            parser(iterator.value(), mnode, renderer, aspect_ratio);
            if ( !parser.mTargetName.empty() )
            {
                mTargets[parser.mCnode] = parser.mTargetName;
            }
            
            mCameras.push_back(parser.mCamera);
        }
        else if ( iterator.key() == "light" )
        {
            LightParser parser;
            parser(iterator.value(), mnode, renderer);
        }
        else if ( iterator.key() == "particle emitter" )
        {
            ParticleEmitterParser parser;
            parser(iterator.value(), mnode, renderer);
        }
        else if ( iterator.key() == "object" )
        {
            ObjectParser parser;
            parser(iterator.value(), mnode, renderer, aspect_ratio);
            // should probably add constraints here, since these are most likely connected bodies
            for ( auto rigid_body : parser.mRigidBodies )
            {
                mRigidBodies.push_back(rigid_body);
            }
            
            for ( auto target_pair : parser.mTargets )
            {
                mTargets[target_pair.first] = target_pair.second;
            }
            
            for ( auto camera : parser.mCameras )
            {
                mCameras.push_back(camera);
            }
        }
    }
    
    parent->AddChildNode(mnode);
}
    
    
}

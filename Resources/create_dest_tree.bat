rem this script is called from Visual Studio

SET solution_dir = %1
SET dest_dir = %solution_dir%\..\..\bin

IF NOT EXIST %dest_dir% MKDIR %dest_dir%
IF NOT EXIST %dest_dir%\Resources MKDIR %dest_dir%\Resources
IF NOT EXIST %dest_dir%\Resources\FXKernels MKDIR %dest_dir%\Resources\FXKernels
IF NOT EXIST %dest_dir%\Resources\Models MKDIR %dest_dir%\Resources\Models
IF NOT EXIST %dest_dir%\Resources\Scripts MKDIR %dest_dir%\Resources\Scripts
IF NOT EXIST %dest_dir%\Resources\Shaders MKDIR %dest_dir%\Resources\Shaders
IF NOT EXIST %dest_dir%\Resources\Textures MKDIR %dest_dir%\Resources\Textures
IF NOT EXIST %dest_dir%\Resources\Fonts MKDIR %dest_dir%\Resources\Fonts
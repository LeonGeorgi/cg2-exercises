@ECHO OFF
cd ..
.\solution\bin\windows\cgv_viewer64_143.exe plugin:cg_fltk "type(shader_config):shader_path='%cd%\solution\glsl'" plugin:crg_stereo_view plugin:cg_icons plugin:cg_ext plugin:cmi_io plugin:CG2_exercise2 "config:'%cd%\solution\config_ex2.cfg'"

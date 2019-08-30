gcc -O2 -o bifurcation_wfm.exe bifurcation_wfm.c -lm
move bifurcation_wfm.exe data
cd data

del filelist

FOR %%i in (*.wfm) do ( echo %%i >> filelist )
rem pause
bifurcation_wfm.exe < filelist
rem pause
del bifurcation_wfm.exe
2D_plot_image.gp
cd ..\
rem pause

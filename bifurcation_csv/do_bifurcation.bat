gcc -O2 -o bifurcation.exe bifurcation.c -lm
move bifurcation.exe data
cd data

del filelist

FOR %%i in (*.csv) do ( echo %%i >> filelist)
rem pause
bifurcation.exe < filelist
rem pause
del bifurcation.exe
2D_plot_image.gp
cd ..\
rem pause

gcc -O2 -o histgram.exe histgram.c -lm
move histgram.exe data
cd data

del filelist

FOR %%i in (*.wfm) do ( echo %%i >> filelist )
rem pause
histgram.exe < filelist
rem pause
del histgram.exe
2D_plot_image.gp
cd ..\
rem pause

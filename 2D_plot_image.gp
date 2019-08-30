set term emf enhanced color "Arial, bold, 24" size									### EMF形式で出力
### set term png enhanced transparent interlace truecolor butt font "Arial, bold, 52" size 2048, 2048
### set term postscript eps enhanced color nobackground lw 2 clip "Arial-bold, 24"
### set term windows enhanced enhanced
set datafile separator ","

unset key	### 凡例を消去

#set origin 0, 1

#set auto fix
#set offsets graph 0, 0, 0, 0

#set size ratio 1
#set lmargin 6	### 左余白
#set rmargin 0	### 右余白
#set tmargin 2	### 上余白
#set bmargin 4	### 下余白

##### ラベルの調整 #####

set ticslevel 0
set tics front
set xtics border nomirror scale 1

set xlabel "MZM1 bias point  {/Symbol f}_{1}" offset 0, 0.7
#set xtics -0.5, 0.5, 1
#set xtics offset -0.2, 0.4
#set xrange [0.0:1.0]
#set mxtics 5
#x_init = 0.5
#x_div = 0.25

set ylabel "MZM2 bias point  {/Symbol f}_{2}" offset 2.5, 0
#set ytics -0.50, 0.5, 1
#set ytics offset 0.2, 0.1
#set yrange [-1.0:1.0]
#set mytics 5
#y_init = -1.0
#y_div = 0.5

#set label 1 center at first 4, graph 1.05 "(c)"

########################

##### line style の調整 #####

set style line 1 linetype 1
set style line 1 linewidth 1
set style line 1 linecolor rgb "black"
set style line 1 pointtype 7
set style line 1 pointsize 1

#############################

##### color bar #####

set logscale cb
set cbrange [0.005:5.0]
#set cbtics 0.01, 0.01, 0.06
#set format cb "10^{%L}"
set cbtics offset -0.5, 0, 0
set palette gray# negative
#set palette defined (-1 "white", 6.0 "black")
#set palette maxcolors 20
#set cbtics 1

#####################

#set output "NMSE_TEST.emf"
#plot "NMSE_TEST.csv" matrix using (x_init + $1 * x_div):(y_init + $2 * y_div):3 with image

#set output "NMSE_TRAIN.emf"
#plot "NMSE_TRAIN.csv" matrix using (x_init + $1 * x_div):(y_init + $2 * y_div):3 with image

set output "NMSE_TRAIN_2D.emf"
plot "NMSE_TRAIN_2D.csv" using 1:2:3 with image

set output "NMSE_TEST_2D.emf"
plot "NMSE_TEST_2D.csv" using 1:2:3 with image

set out
set term pop

#pause -1 "Hit return to continue"

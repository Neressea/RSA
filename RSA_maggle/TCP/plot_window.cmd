set terminal post eps color solid "Helvetica" 28
set key right bottom
set grid xtics ytics

set output "courbes/cwnd.eps"
set xlabel "temps"
set ylabel "cwnd"
plot "trace_file.tr" u 1:2 notitle with linespoints lw 2


data = csvread('graph.csv');
t = data(2:end, 1);
y1_pid = data(1, 2);
y2_pid = data(1, 3);
y1 = data(2:end, 2);
y2 = data(2:end, 3);
f = figure
plot(t, y1)
hold on
plot(t, y2)
legend("PID 5(100 tickets)", "PID 6(200 tickets)");
xlabel("Time(ticks)")
ylabel("Accumulated Ticks")


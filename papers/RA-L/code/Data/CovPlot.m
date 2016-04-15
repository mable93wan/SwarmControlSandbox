load CovPlot3Cross.txt;
t1 = CovPlot3Cross(:,1);
C1 = CovPlot3Cross(:,2);
GC1 = CovPlot3Cross(:,3);
Vx1 = CovPlot3Cross(:,4);
GVx1 = CovPlot3Cross(:,5);
Vy1 = CovPlot3Cross(:,6);
GVy1 = CovPlot3Cross(:,7);
load CovPlot4Cross.txt;
t2 = CovPlot4Cross(:,1);
C2 = CovPlot4Cross(:,2);
GC2 = CovPlot4Cross(:,3);
Vx2 = CovPlot4Cross(:,4);
GVx2 = CovPlot4Cross(:,5);
Vy2 = CovPlot4Cross(:,6);
GVy2 = CovPlot4Cross(:,7);

load CovPlot6Cross.txt;
t3 = CovPlot6Cross(:,1);
C3 = CovPlot6Cross(:,2);
GC3 = CovPlot6Cross(:,3);
Vx3 = CovPlot6Cross(:,4);
GVx3 = CovPlot6Cross(:,5);
Vy3 = CovPlot6Cross(:,6);
GVy3 = CovPlot6Cross(:,7);


subplot(3,1,1)
plot(t1,GC1, 'color','g')
hold on
plot(t1,C1,'color','b')
plot(t2,C2,'color','m')
plot(t3,C3,'color','c')
hold off

subplot(3,1,2)

plot(t1,GVx1, 'color','g')
hold on
plot(t1,Vx1,'color','b')
plot(t2,Vx2,'color','m')
plot(t3,Vx3,'color','c')
hold off

subplot(3,1,3)

plot(t1,GVy1, 'color','g')
hold on
plot(t1,Vy1,'color','b')
plot(t2,Vy2,'color','m')
plot(t3,Vy3,'color','c')
hold off
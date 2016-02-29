% load TSC10D.txt;
% x = TSC10D(:,1);
% y = TSC10D(:,2);
% plot(x,y)
% hold on
% load TSC10S.txt;
% x2 = TSC10S(:,1);
% y2 = TSC10S(:,2);
% plot(x2,y2)
% 
% load TSC10Up.txt;
% x3 = TSC10Up(:,1);
% y3 = TSC10Up(:,2);
% plot(x3,y3)
% 
% 
% % load Straight4.txt;
% % x4 = Straight4(:,1);
% % y4 = Straight4(:,2);
% % plot(x4,y4)
% % 
% % load Straight5.txt;
% % x5 = Straight5(:,1);
% % y5 = Straight5(:,2);
% % plot(x5,y5)
% line([11 30] , [0 0]);
% hold off
% load OrientationCont2.txt;
% x = OrientationCont2(:,1);
% y = OrientationCont2(:,2);
% z = OrientationCont2(:,3);
% plot(x,y)
% hold on
% plot (x,z)
% 
% load OrientationCont3.txt;
% x2 = OrientationCont3(:,1);
% y2 = OrientationCont3(:,2);
% z2 = OrientationCont3(:,3);
% plot(x2,y2)
% 
% plot (x2,z2)
% load OrientationCont4.txt;
% x3 = OrientationCont4(:,1);
% y3 = OrientationCont4(:,2);
% z3 = OrientationCont4(:,3);
% plot(x3,y3)
% 
% plot (x3,z3)
% hold off

load PivotQuart.txt;
x = PivotQuart(:,1);
y = PivotQuart(:,2);

plot(x,y, 'color','green')
hold on


load PivotHalf.txt;
x2 = PivotHalf(:,1);
y2 = PivotHalf(:,2);
plot(x2,y2, 'color', 'blue')

load PivotFull.txt;
x3 = PivotFull(:,1);
y3 = PivotFull(:,2);

plot(x3,y3, 'color', 'red')

load PivotZero.txt;
x4 = PivotZero(:,1);
y4 = PivotZero(:,2);

plot(x4,y4, 'color', 'cyan')
hold off
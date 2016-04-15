load FrictionLess.txt;
t1 = FrictionLess(:,1);
C1 = FrictionLess(:,2)*5;

load FrictionFew.txt;
C2 = FrictionFew(:,1)*5;
t2 = FrictionFew(:,2);

load FrictionBigger.txt;
C3 = FrictionBigger(:,1)*5;
t3 = FrictionBigger(:,2);

load FrictionData.txt;
t4 = FrictionData(:,1);
C4 = FrictionData(:,2)*5;

plot(t1,C1, 'color','b')
hold on
plot(t3,C3,'color','m')
plot(t2,C2 ,'color','r')


plot(t4,C4,'color','g')
hold off
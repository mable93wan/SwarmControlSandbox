load CovControl.txt;
t = CovControl(:,1);
C = CovControl(:,2);
G = CovControl(:,3);

plot(t,C,'color','b')
hold on
plot(t,G, 'color','g')
hold off
figure
load CovControl2.txt;
t2 = CovControl2(:,1);
C2 = CovControl2(:,2);
G2 = CovControl2(:,3);

plot(t2,C2,'color','b')
hold on
plot(t2,G2, 'color','g')









goal1x = 800;
goal2x = 600;
goalX = goal1x;

t = timer('TimerFcn',...
    'goalX = my_callback_fcn(goalX, goal1x, goal2x);' , ...
    'Period' , 2, 'TasksToExecute' , 10, 'ExecutionMode', 'fixedSpacing');
 
start(t);
t0 = tic;

q = zeros(1,2);
tval = toc(t0);
while(tval < 10)
   tval = toc(t0);
   newVal = [tval, goalX];
   q= [q;newVal];
end
plot(q(:,1),q(:,2));
hold on

hold off
 

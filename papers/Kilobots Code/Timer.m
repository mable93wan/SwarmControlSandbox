function TimerShiva()
%cam = webcam(2);
global q goalX
goalY = 434;
goalXM = 600;
goalC1 = [18000 -4000; -4000 18000];
goalC2 = [18000 4000; 4000 18000];
goal1x = goalC1(1,2);
goal2x = goalC2(1,2);
goalX = goal1x;
t0 = tic;
q = zeros(1,2);

success = false;

tHandle = timer('TimerFcn',...
    {@sqWave_callback_fcn, goal1x, goal2x,t0}, ...
    'Period' , 100, 'TasksToExecute' , 4, 'ExecutionMode', 'fixedDelay');
 
start(tHandle);


drawTime=[goalX,0];

    if(toc(t0) > 810)
        hold on
       plot(q(:,1),q(:,2));

        
hold off
    end


stop(tHandle)
 
function  sqWave_callback_fcn(src,evt, goal1x, goal2x,t0) %#ok<DEFNU>

   tval = toc(t0);
    q= [q;[tval, goalX]];
    
    if goalX == goal1x
        goalX = goal2x;
    else
        goalX = goal1x;
    end
    q= [q;[tval, goalX]];
    
%     figure(1)
% plot(q(:,1),q(:,2));
% xlabel('time (s)');
% ylabel('pixels');

end

end

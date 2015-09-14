function TimerShiva()
cam = webcam(2);
global q goalX
goalY = 434;
goalXM = 600;
goalC1 = [18000 -12000; -12000 18000];
goalC2 = [18000 12000; 12000 18000];
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
while success == false

    
    % Read in a webcam snapshot.
    originalImage = snapshot(cam);
    imwrite(originalImage,'FailImage.png');
    
    originalImage = imcrop(originalImage,[345 60 1110 850]);
    % make grayscale.
    originalImage = rgb2gray(originalImage);
    BW = im2bw(originalImage, 0.2);  %threshold the image to remove shadows (and only show dark parts of kilobots)
    [centers, radii] = imfindcircles(BW,[13 19],'ObjectPolarity','dark','Sensitivity',0.95 );
    %[centers, radii] = imfindcircles(originalImage,[12 20],'ObjectPolarity','dark','Sensitivity',0.93);
    M = mean(centers);
    V = var(centers);
    C = cov(centers);
    
    imshow(originalImage);
    h = viscircles(centers,radii);
    [s, l] = size(centers);
    goalC = [18000 goalX; goalX 18000];
    
    if isnan(M)== false 
        
    if s > 5 
    hold on
    again = false;
    plot(M(1,1) , M(1,2),'*','Markersize',16);
    newDot = [C(1,2), toc(t0)];
    drawTime = [drawTime;newDot];
    plot_gaussian_ellipsoid(M,C);
    plot_gaussian_ellipsoid([goalXM goalY],goalC);
    plot(centers(:,1),centers(:,2),'+','Markersize',16);
    %plot(goalX, 'Markersize',16);
    line([goalXM goalXM], ylim,'color','green');
    hold off
    end
    end

    if(toc(t0) > 410)
        success = true;
        figure(2)
       plot(q(:,1),q(:,2));
        xlabel('time (s)');
        ylabel('k pixels');
        
        hold on 
        plot(drawTime(:,2), drawTime(:,1));
        hold off
        
       clear('cam')
    end
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

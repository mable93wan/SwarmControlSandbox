%%% Controlling mean of kilobots with light by using a webcam, by: Shiva
%%% Shahrokhi and Aaron T. Becker @ University of Houston, Robotic Swarm
%%% Control Lab.

% Using Arduino for our lamps, this is how we define arduino in Matlab:

% Defining webcam
cam = webcam(2);


%These are the goal positions for Mean Control
goalX = 800;
goalY = 434;
epsilon = 10;
% We will do this until we reach the goal.
success = false;

goal1x = 700;
goal2x = 600;
goalX = goal1x;


drawTime=[goalX,0,goalX];



t0 = tic;
while success == false

    
    % Read in a webcam snapshot.
    originalImage = snapshot(cam);
    originalImage = imcrop(originalImage,[345 60 1110 850]);
    % make grayscale.
    originalImage = rgb2gray(originalImage);
    BW = im2bw(originalImage, 0.25);  %threshold the image to remove shadows (and only show dark parts of kilobots)
    [centers, radii] = imfindcircles(BW,[12 19],'ObjectPolarity','dark','Sensitivity',0.94 );
    %[centers, radii] = imfindcircles(originalImage,[12 20],'ObjectPolarity','dark','Sensitivity',0.93);
    M = mean(centers);
    V = var(centers);
    C = cov(centers);
    imshow(originalImage);
    h = viscircles(centers,radii);
    [s, l] = size(centers);
    goalC = [4 2; 2 4];
    
    if isnan(M)== false 
        
    if s > 50 
    hold on
    again = false;
    plot(M(1,1) , M(1,2),'*','Markersize',16);
    newDot = [C(1,2), toc(t0), goalC(1,2)];
    drawTime = [drawTime;newDot];
    plot_gaussian_ellipsoid(M,C);
%    plot_gaussian_ellipsoid([goalX goalY],goalC);
    plot(centers(:,1),centers(:,2),'+','Markersize',16);
    %plot(goalX, 'Markersize',16);
    line([goalX goalX], ylim);
    hold off
    end
    end
end
    if(toc(t0) > 310)
        success = true;
        figure
        plot(drawTime(:,2), drawTime(:,1));
        hold on 
        plot(drawTime(:,2), drawTime(:,3));
        hold off
        
       clear('cam')
    end


% Read in a webcam snapshot.

originalImage = snapshot(cam);




%%% Controlling mean of kilobots with light by using a webcam, by: Shiva
%%% Shahrokhi and Aaron T. Becker @ University of Houston, Robotic Swarm
%%% Control Lab.

% Using Arduino for our lamps, this is how we define arduino in Matlab:
a = arduino('/dev/tty.usbmodem1411','uno');
% Defining webcam
cam = webcam(2);

% We have 8 Relays.
%This is y direction Straigt 
RELAY1 = 7;
% This is x Direction Left
RELAY2 = 6;
%This is y direction backwards
RELAY3 = 5;
% This is X direction Right
RELAY4 = 4;

RELAY5 = 3;

RELAY6 = 2;
RELAY7 = 1;

%This Relay is used for flash for taking pictures.
RELAY8 = 0;

%These are the goal positions for Mean Control

goalY = 434;
epsilon = 10;
% We will do this until we reach the goal.
success = false;
writeDigitalPin(a, RELAY1, 0);
    pause (1);
    again =false;
goal1x = 700;
goal2x = 600;
goalX = goal1x;

T = 100;
numberOfIteration = 10;
numberOfSeconds = T * numberOfIteration;
accuracy = 10;
n = numberOfSeconds* accuracy;
drawTime=[goalX,0,goalX];


t = timer('TimerFcn',...
    'goalX = my_callback_fcn(goalX, goal1x, goal2x);' , ...
    'Period' , T, 'TasksToExecute' , 10, 'ExecutionMode', 'fixedDelay');
 
start(t);
t0 = tic;
while success == false
    if again== true
        writeDigitalPin(a, RELAY4, 0);
    pause (1);
    end 
    
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
    
    if isnan(M)== false 
        
    if s > 50 
    hold on
    again = false;
    plot(M(1,1) , M(1,2),'*','Markersize',16);
    newDot = [M(1,1), toc(t0), goalX];
    drawTime = [drawTime;newDot];
    plot_gaussian_ellipsoid(M,C);
    plot(centers(:,1),centers(:,2),'+','Markersize',16);
    %plot(goalX, 'Markersize',16);
    line([goalX goalX], ylim);
    hold off
    
    if M(1,1) > goalX+epsilon
        writeDigitalPin(a,RELAY4,1);
        writeDigitalPin(a, RELAY1,0);
        writeDigitalPin(a,RELAY3,1);
       
    else if M(1,1) < goalX-epsilon
            writeDigitalPin(a,RELAY4,1);
        writeDigitalPin(a, RELAY1,1);
        writeDigitalPin(a,RELAY3,0);
       
        else 
            writeDigitalPin(a,RELAY2,1);
            writeDigitalPin(a, RELAY1,1);
            writeDigitalPin(a,RELAY3,1);
            again = true;
        end
    end
    else 
        again = true;
    end
end
    if(toc(t0) > 310)
        success = true;
        figure
        plot(drawTime(:,2), drawTime(:,1));
        hold on 
        plot(drawTime(:,2), drawTime(:,3));
        hold off
        
        clear('cam');
        delete(a);
    end
end

% Read in a webcam snapshot.

originalImage = snapshot(cam);




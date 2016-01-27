
%%% Controlling Open Loop Covariance of kilobots with light by using a webcam, by: Shiva
%%% Shahrokhi and Aaron T. Becker @ University of Houston, Robotic Swarm
%%% Control Lab.

function OpenLoop

% Using Arduino for our lamps, this is how we define arduino in Matlab:
a = arduino('/dev/tty.usbmodem1411','uno');
%Define webcam --the input may be 1 or 2 depending on which webcam of your laptop
%is the default webcam.
cam = webcam(2);


% We have 8 Relays.
%west
RELAY1 = 9;
% northwest
RELAY2 = 8;
%north
RELAY3 = 5;
% northeast
RELAY4 = 2;
%east
RELAY5 = 7;
%southeast
RELAY6 = 3;
%south
RELAY7 = 6;
%southwest
RELAY8 = 4;

%These are the goal X.


t0 = tic;

success = false;
again = true;
    Time = 0;
    CycleTime = 180;

drawTime=[0,0];
drawTime2 =[0,0];
while success == false

    if again== true
        writeDigitalPin(a, RELAY1,0);
        writeDigitalPin(a,RELAY2,0);
        writeDigitalPin(a,RELAY3,0);
        writeDigitalPin(a,RELAY4,0);
        writeDigitalPin(a, RELAY5,0);
        writeDigitalPin(a,RELAY6,0);
        writeDigitalPin(a,RELAY7,0);
        writeDigitalPin(a,RELAY8,0);
    pause (1);
    end 
    % Read in a webcam snapshot.
    rgbIm = snapshot(cam);
    %imwrite(rgbIm,'FailImage6.png');
    %crop to have just the table view.
originalImage = imcrop(rgbIm,[345 60 1110 850]);
orig = originalImage;
% make grayscale.
I = rgb2hsv(originalImage);


% Define thresholds for channel 1 based on histogram settings
channel1Min = 0.065;
channel1Max = 0.567;

% Define thresholds for channel 2 based on histogram settings
channel2Min = 0.288;
channel2Max = 1.000;

% Define thresholds for channel 3 based on histogram settings
channel3Min = 0.400;
channel3Max = 1.000;

% Create mask based on chosen histogram thresholds
BW = (I(:,:,1) >= channel1Min ) & (I(:,:,1) <= channel1Max) & ...
    (I(:,:,2) >= channel2Min ) & (I(:,:,2) <= channel2Max) & ...
    (I(:,:,3) >= channel3Min ) & (I(:,:,3) <= channel3Max);

  %threshold the image to remove shadows (and only show dark parts of kilobots)
    [centers, radii] = imfindcircles(BW,[10 19],'ObjectPolarity','bright','Sensitivity',0.92 );
    
    % %Mean
    M = mean(centers);
    %Variance
    %V = var(centers);
    %Covariance
    C = cov(centers);
    
    imshow(originalImage);
    viscircles(centers,radii,'EdgeColor','b');
    [s, l] = size(centers);

    
    if isnan(M)== false 
        if s < 95
            filename = strcat('FailImage',s);
            imwrite(orig,strcat(filename,'.png'));
        end
        
    if s > 5 
        again = false;
    hold on
    
    plot(M(1,1) , M(1,2),'*','Markersize',16,'color','red', 'linewidth',3);
%    plot(goalX , goalY,'*','Markersize',16,'color','green','linewidth',3);
    newDot = [C(1,2), toc(t0)];
    newDot2 = [C(2,1), toc(t0)];
    drawTime = [drawTime;newDot];
    drawTime2 = [drawTime2;newDot2];
    save('covDataX','drawTime');
    save('covDataY','drawTime');
    %Current Mean and Covariance Ellipse
    plot_gaussian_ellipsoid(M,C);
    %Goal Mean and Covariance Ellipse
    %plot_gaussian_ellipsoid([goalXM goalYM],goalC);
    %plot(centers(:,1),centers(:,2),'+','Markersize',16);
    %Goal X.
    %line([goalX goalX], ylim,'color','green','linewidth', 3.0);
    
    %Started at 66cm * 17 cm 
    hold off
    delayTime = 3;

        if (toc(t0) < Time + CycleTime/3) 
                writeDigitalPin(a, RELAY1,1);
        writeDigitalPin(a,RELAY2,1);
        writeDigitalPin(a,RELAY3,1);
        writeDigitalPin(a,RELAY4,1);
        writeDigitalPin(a, RELAY5,0);
        writeDigitalPin(a,RELAY6,1);
        writeDigitalPin(a,RELAY7,1);
        writeDigitalPin(a,RELAY8,1);
                pause(delayTime);
       
    else if (toc(t0) < Time + CycleTime*2/3) 
            writeDigitalPin(a, RELAY1,1);
        writeDigitalPin(a,RELAY2,1);
        writeDigitalPin(a,RELAY3,1);
        writeDigitalPin(a,RELAY4,0);
        writeDigitalPin(a, RELAY5,1);
        writeDigitalPin(a,RELAY6,1);
        writeDigitalPin(a,RELAY7,1);
        writeDigitalPin(a,RELAY8,1);
                pause(delayTime);
       
        else  if (toc(t0) <Time +  CycleTime*3/4) 
         writeDigitalPin(a, RELAY1,0);
        writeDigitalPin(a,RELAY2,1);
        writeDigitalPin(a,RELAY3,1);
        writeDigitalPin(a,RELAY4,1);
        writeDigitalPin(a, RELAY5,1);
        writeDigitalPin(a,RELAY6,1);
        writeDigitalPin(a,RELAY7,1);
        writeDigitalPin(a,RELAY8,1);
                pause(delayTime);
       
    else if (toc(t0) < Time + CycleTime)
             writeDigitalPin(a, RELAY1,1);
        writeDigitalPin(a,RELAY2,1);
        writeDigitalPin(a,RELAY3,1);
        writeDigitalPin(a,RELAY4,1);
        writeDigitalPin(a, RELAY5,1);
        writeDigitalPin(a,RELAY6,1);
        writeDigitalPin(a,RELAY7,1);
        writeDigitalPin(a,RELAY8,0);
                pause(delayTime);
        else
            Time = Time + CycleTime;
                  
        end
              end
        end
        end
    end
    
    end

    if(toc(t0) > CycleTime * 5)
        success = true;

        plot(drawTime(:,2), drawTime(:,1));
            save('covDataX','drawTime');
    save('covDataY','drawTime');
        hold off
        
       clear('cam')
    end
end

 


end

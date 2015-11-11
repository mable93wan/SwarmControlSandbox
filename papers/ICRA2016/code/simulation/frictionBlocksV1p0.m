function frictionBlocksV1p0
% Blocks move according to the keyboard input unless the collide with walls
% or another robot.  If a robot is touching a bottom wall, it can only move
% up, if touching right wall, only move left, etc.  
%
%(MAYBE) We will allow 8-point
% moves.  (if you hold onto the top and right buttom, move in diagonal
% Author Names and email
%  Arun ...   and Aaron T. Becker
%                    atbecker@uh.edu
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

global G MAKE_MOVIE RobotPts
%close all; clc;
clear all
clc
format compact
MAKE_MOVIE = false;
G.fig = figure(1);
G.numCommands = 0;
G.totalMoves = 0;
if MAKE_MOVIE
    set(G.fig ,'Name','Massive Control','color',[1,1,1]); %#ok<UNRCH>
else
    set(G.fig ,'KeyPressFcn',@keyhandler,'Name','Massive Control');
end


%[G.obstacle_pos,RobotPts] = simpleConverge(15,20);
[G.obstacle_pos,RobotPts] = SetupWorld();

G.EMPTY = 0;
G.OBST = 1;


G.maxX = size(G.obstacle_pos,2);
G.maxY = size(G.obstacle_pos,1);

%create vector of robots and draw them.  A robot vector consists of an xy
%position, an index number, and a color.

numRobots = size(RobotPts,1);
initP = RobotPts;
figure(1)
clf

G.colormap = [  1,1,1; %Empty = white
    0,0,0; %obstacle
    0.5,0.5,0.5;
    ];
colormap(G.colormap(1:numel(unique(G.obstacle_pos)),:));
G.axis=imagesc(G.obstacle_pos);
set(gca,'box','off','xTick',[],'ytick',[],'ydir','normal','Visible','off');

%set(G.axis,'edgealpha',.08)
axis equal
%axis tight
hold on
%set(G.fig,'Units','normalized','outerposition',[0 0 1 1]);%'MenuBar','none',

%end
G.hRobots = zeros(1, numRobots);
colors = jet(numel(unique(RobotPts(:,4)))+1);
%colors = [1,0,0;0,0,1; 1,.5,1; .5,0,.5;];


for hi = 1: numRobots
    if numel( RobotPts(hi,:))>5
        G.hRobots(hi) =  rectangle('Position',[RobotPts(hi,1)-1/2,RobotPts(hi,2)-1/2,RobotPts(hi,5),RobotPts(hi,6)],'Curvature',[1/RobotPts(hi,5),1/RobotPts(hi,6)],'FaceColor',colors(RobotPts(hi,4),:));
    else
        G.hRobots(hi) =  rectangle('Position',[RobotPts(hi,1)-1/2,RobotPts(hi,2)-1/2,1,1],'Curvature',[1,1],'FaceColor',colors(RobotPts(hi,4),:));
    end
end

%%%%%%%automatic code  (TODO)
mvs = ['w','d','d','d','x','a'];
for myc = 1:numel(mvs)
    pause(.25)
%   moveto(mvs(myc))
end
%%%%%%%%



    function keyhandler(src,evnt) %#ok<INUSL>
        if strcmp(evnt.Key,'s')
            imwrite(flipud(get(G.axis,'CData')+1), G.colormap, '../../pictures/png/MatrixPermutePic.png');
        else
            moveto(evnt.Key)
        end
    end

function retVal = frictionOK(stVal, step, G)
    frictionBad = (stVal(2)==2 && step(2) <1 ) ... %friction if on bottom row and not trying to move up
        || (stVal(1) == G.maxX-1 && step(1) >-1) ; %friction if on right wall and not trying to move left
    %TODO: add friction in all directions
    retVal = ~frictionBad;
end

    function retVal = spaceFreeWithNoRobot(desVal, RobotPts, G)
        % move there if no robot in the way and space is free
        retVal =  ~ismember(desVal,RobotPts(:,1:2),'rows')  ...
            && desVal(1) >0 && desVal(1) <= G.maxX && desVal(2) >0 && desVal(2) <= G.maxY ... %check that we are not hitting the boundary
            && G.obstacle_pos( desVal(2),desVal(1) )~=1; %check we are not hitting the obstacle        
    end

    function moveto(key)
        % Maps keypresses to moving pixels
        step = [0,0];
        if strcmp(key,'r')  %RESET
            RobotPts = initP;
            for ni = 1:size(RobotPts,1)
                set(G.hRobots(RobotPts(ni,3)),'Position',[RobotPts(ni,1)-1/2,RobotPts(ni,2)-1/2,1,1]);
                uistack(G.hRobots(RobotPts(ni,3)),'top');
            end
            return
        end
        
        if strcmp(key,'leftarrow') || strcmp(key,'a') %-x
            RobotPts = sortrows(RobotPts,1);
            step = -[1,0];
        elseif strcmp(key,'e') %+xy  %TODO: add other diagonal moves
            RobotPts = sortrows(RobotPts,1);
            step = [1,1];
        elseif strcmp(key,'rightarrow')|| strcmp(key,'d') %+x
            RobotPts = sortrows(RobotPts,-1);
            step = [1,0];
        elseif strcmp(key,'uparrow')|| strcmp(key,'w') %+y
            RobotPts = sortrows(RobotPts,-2);
            step = [0,1];
        elseif strcmp(key,'downarrow')|| strcmp(key,'x') %-y
            RobotPts = sortrows(RobotPts,2);
            step = -[0,1];
        end
        
        % implement the move on every robot
        for ni = 1:size(RobotPts,1)
            stVal = RobotPts(ni,1:2);
            desVal = RobotPts(ni,1:2)+step;
            
            % move there if no robot in the way and space is free
            if spaceFreeWithNoRobot(desVal, RobotPts, G) && frictionOK(stVal, step, G)
                RobotPts(ni,1:2) = desVal;
            end
            %redraw the robot
            if ~isequal( stVal, RobotPts(ni,1:2) )
                if numel( RobotPts(hi,:))>5
                    set(G.hRobots(RobotPts(ni,3)),'Position',[RobotPts(ni,1)-1/2,RobotPts(ni,2)-1/2,RobotPts(ni,5),RobotPts(ni,6)]);
                else
                    set(G.hRobots(RobotPts(ni,3)),'Position',[RobotPts(ni,1)-1/2,RobotPts(ni,2)-1/2,1,1]);
                end
            end  
        end        
    end

    function [blk,RobotPts] = SetupWorld()
        
        numrobots = 8;
        
        blk=[1 1 1 1 1 1 1 1 1 1 1 1;
            1 0 0 0 0 0 0 0 0 0 0 1;
            1 0 0 0 0 0 0 2 2 2 0 1;
            1 0 0 0 0 0 0 2 0 2 0 1;
            1 0 0 0 0 0 0 2 2 2 0 1;
            1 0 0 0 0 0 0 0 0 0 0 1;
            1 0 0 0 0 0 0 0 0 0 0 1;
            1 0 0 0 0 0 0 0 0 0 0 1;
            1 0 0 0 0 0 0 0 0 0 0 1;
            1 0 0 0 0 0 0 0 0 0 0 1;
            1 0 0 0 0 0 0 0 0 0 0 1;
            1 1 1 1 1 1 1 1 1 1 1 1;];
        blk = flipud(blk);
        
        pos = zeros(numrobots,2);
        %randomly place robots not on obstacles and not overlapping
        for i=1:numrobots
            pos(i,:) = [5+i-4*floor((i-1)/4), floor((i-1)/4)+3];
        end
        RobotPts = [pos,(1:numrobots)',(1:numrobots)'];
    end

end
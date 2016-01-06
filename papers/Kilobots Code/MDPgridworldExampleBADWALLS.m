function [V_hat_prob, DX,DY] = MDPgridworldExampleBADWALLS(map, goalX, goalY)
%  Applies value iteration to learn a policy for a Markov Decision Process
%  (MDP) -- a robot in a grid world.
% 
% The world is freespaces (0) or obstacles (1). Each turn the robot can
% move in 8 directions, or stay in place. A reward function gives one
% freespace, the goal location, a high reward. All other freespaces have a
% small penalty, and obstacles have a large negative reward. Value
% iteration is used to learn an optimal 'policy', a function that assigns a
% control input to every possible location.
% 
% This function compares a deterministic robot, one that always executes
% movements perfectly, with a stochastic robot, that has a small
% probability of moving +/-45degrees from the commanded move.  The optimal
% policy for a stochastic robot avoids narrow passages and tries to move to
% the center of corridors.
% 
% From Chapter 14 in 'Probabilistic Robotics', ISBN-13: 978-0262201629,
% http://www.probabilistic-robotics.org
% 
%  Aaron Becker, March 11, 2015

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
ftsz = 22;
set(0,'DefaultAxesFontSize',ftsz)
format compact

% pauseOn = false;  %setting this to 'true' is useful for teaching, because it pauses between each graph
World = map;
f1 = figure(1); clf
set(f1,'units','normalized','outerposition',[0 0 1 1])
colormap(gray)
%colormap(jet)
R = -5000000*ones(size(World)); %-50 reward for obstacles
R(World==0) = -1; %small penalty for not being at the goal
R(goalX,goalY) = 10; %goal state has big reward

% %  DRAW THE WORLD, REWARD, ANIMATE VALUE ITERATION, DISPLAY POLICY
% subplot(2,2,1)
% imagesc(~World);
% set(gca,'Xtick',[], 'Ytick',[])
% axis equal
% axis tight
% text(25,-1,'World','HorizontalAlignment','center','FontSize',ftsz)
% drawnow
% if pauseOn; pause(); end %#ok<*UNRCH>

% subplot(2,2,2)
% imagesc(R);
% axis equal
% axis tight
% set(gca, 'Xtick',[], 'Ytick',[])
% text(25,-1,'Reward function','HorizontalAlignment','center','FontSize',ftsz)
% drawnow
% if pauseOn; pause(); end

% 
% V_hat = MDP_discrete_value_iteration(R,World,false);
% if pauseOn; pause(); end

% DrawPolicy(V_hat,World,false);
% if pauseOn; pause(); end

figure(f1)
[V_hat_prob, DX, DY] = MDP_discrete_value_iteration(R,World,true);
% if pauseOn; pause(); end


% DrawPolicy(V_hat_prob,World,true);
% if pauseOn; pause(); end

    function [V_hat, DX,DY] = MDP_discrete_value_iteration(R,World,prob)
        % iterates on the value function approximation V_hat until the V_hat converges.
        V_hat_prev = zeros(size(World));

        V_hat = -100*ones(size(World));
        V_hat(World==0) = R(World==0);
        colormap(jet)
        gamma = 0.99;
        
        xIND = find(World == 0);
        [X,Y] = meshgrid(1:size(World,2),1:size(World,1));
        DX = zeros(size(X));
        DY = zeros(size(Y));
        iteration = 0;
        if ~prob
            subplot(2,2,3)
        else
            subplot(2,2,4)
        end
        hImageV =   imagesc(V_hat);
        axis equal
        axis tight
        set(gca,'Xtick',[], 'Ytick',[])
        htext = text(25,-1,'Vhat','HorizontalAlignment','center','FontSize',ftsz);
        iteration_limit = 200; %value function needs ~600 iterations to converge, but the policy converges after ~100 iterations
        while ~isequal(V_hat,V_hat_prev) && iteration < iteration_limit 
            V_hat_prev = V_hat;
            for i = 1:numel(xIND)   
                [bestMove,bestPayoff] = policy_MDP(xIND(i),V_hat,prob);
                rowL = size(V_hat,1);
                rowV = xIND(i)/rowL;
                Ix = ceil(rowV);
                Iy =  xIND(i)- rowL* floor(rowV);
                DX(Iy,Ix) = bestMove(1);
                DY(Iy,Ix) = bestMove(2);
                V_hat(xIND(i)) = gamma*( R(xIND(i)) + bestPayoff );
            end
            iteration = iteration+1;
            set(hImageV,'cdata',V_hat);
            set(htext,'String',['Vhat, iteration ',num2str(iteration)])
            %drawnow
        end
    end

    function [bestMove,bestPayoff] = policy_MDP(index,V_hat,prob)
        %computes the best control action, the (move) that generates the
        %most (payoff) according to the current value function V_hat
        %[Iy,Ix] = ind2sub(size(V_hat),index);
        rowL = size(V_hat,1);
        rowV = index/rowL;
        Ix = ceil(rowV);
        Iy =  index- rowL* floor(rowV);
        moves = [1,0; 1,1; 0,1; -1,1; -1,0; -1,-1; 0,-1; 1,-1; 0,0]; 
        bestPayoff = -200; %negative infinity
        %probStraight = 0.8;
        for k = [1,3,5,7,2,4,6,8,9]% This order tries straight moves before diagonals %1:size(moves,1) %
            move = [moves(k,1),moves(k,2)];
            if ~prob
                payoff = V_hat(Iy+move(1),Ix+move(2));
            else    
   
                    iR = mod(k,8)+1;
                    moveR = [moves(iR,1),moves(iR,2)];
                    iRR = mod(k+1,8)+1;
                    moveRR = [moves(iRR,1),moves(iRR,2)];
                    iRRR = mod(k+2,8)+1;
                    moveRRR = [moves(iRRR,1),moves(iRRR,2)];
               
                    iL = mod(k-2,8)+1;
                    moveL = [moves(iL,1),moves(iL,2)]; 
                    iLL = mod(k-3,8)+1;
                    moveLL = [moves(iLL,1),moves(iLL,2)]; 
                    iLLL = mod(k-3,8)+1;
                    moveLLL = [moves(iLLL,1),moves(iLLL,2)];  
                payoff =  .3*V_hat(Iy+move(1), Ix+move(2) )+...
                    .15*V_hat(Iy+moveR(1),Ix+moveR(2))+...
                    .15*V_hat(Iy+moveL(1),Ix+moveL(2))+...
                    .1*V_hat(Iy+moveRR(1),Ix+moveRR(2))+...
                    .1*V_hat(Iy+moveLL(1),Ix+moveLL(2))+...
                    .1*V_hat(Iy+moveRRR(1),Ix+moveRRR(2))+...
                    .1*V_hat(Iy+moveLLL(1),Ix+moveLLL(2));
            end
            
            if payoff > bestPayoff
                bestPayoff = payoff;
                bestMove = move;
            end
        end
    end

    function [DX,DY] = DrawPolicy(V_hat,World,prob)
        % uses arrows to draw the optimal policy according to the Value
        % Funtion approximation V_hat
        xIND = find(World == 0);
        %subplot(3,2,4)
        fh = figure(); clf
        %colormap(gray)
        set(fh,'units','normalized','outerposition',[0 0 1 1])
        imagesc(V_hat);
        axis equal
        axis tight
        set(gca,'Xtick',[], 'Ytick',[])
        if prob
            str = 'Policy under probabilistic motion model';
        else
            str = 'Policy under deterministic motion model';
        end
        text(25,-1,str,'HorizontalAlignment','center','FontSize',ftsz);
        [X,Y] = meshgrid(1:size(World,2),1:size(World,1));
        DX = zeros(size(X));
        DY = zeros(size(Y));
        
        for i = 1:numel(xIND)
            [Iy,Ix] = ind2sub(size(V_hat),xIND(i));
            [bestMove,~] = policy_MDP(xIND(i),V_hat,prob);
            DX(Iy,Ix) = bestMove(1);
            DY(Iy,Ix) = bestMove(2);
        end
        hold on; hq=quiver(X,Y,DY,DX,0.5,'color',[0,0,0]); hold off
        set(hq,'linewidth',2);
        drawnow
    end

end


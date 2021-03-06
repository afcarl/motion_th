function stimParams = motion_th_params_teach(ID)

%This determines the location in which the stimulus will appear: 
stimParams.locat=[ceil(rand*2) ceil(rand*2) ceil(rand*2) ceil(rand*2)]; %Choose some random location (1 or 2)

%This will determine the directions used in this run: 
stimParams.dotDirections = linspace(45,315,4); %All oblique directions for the teaching

stimParams.regions = 8;
stimParams.size = [1.5 3.1];
stimParams.RingWidth = .1;
stimParams.SpokeWidth = .1;
stimParams.StaRange = 1;     			% color map values reserved for ring
stimParams.cueDuration = .75;			% seconds
stimParams.delayDuration = 0.75;		% seconds
stimParams.stimulusDuration = 1.500;    % seconds 
stimParams.interStimulusDuration = 0.200; %seconds
stimParams.responseDuration = 3.625;	% seconds
stimParams.feedbackDuration = 0.25; 	% seconds
stimParams.fixationPoint = 1;
stimParams.id = ID;			  	% Subject's initials
stimParams.feedback = 1;
stimParams.dummyScans = 0;          % Add one block (six trials) of dummy scans
stimParams.numberScans = 4;         %Each scan is stimParams.numberTrials long
stimParams.numOfTrials = 10;        %Each scan is stimParams.numberTrials long

stimParams.fixationSize = .25;      %width and height (degrees of visual angle) 
stimParams.type = 'ring';         %type of fixation point
stimParams.dotDensity = 7;         %pixels/dot - lower values means denser! a value of seven equals about 17 dots per degree squared for 1600x1200 screen resolution
                                    %a value of seven equals about 8.5 dots per degree squared for 800x600 screen resolution
%stimParams.dotVelocity = 0.01;     %degrees per frame
stimParams.dotVelocity = 8;         %degrees per second
stimParams.dotSize = 0.03;          %equivalent to 2 x 2 pixels for 800 x 600 monitor
%stimParams.dotSize = 0.02;         %equivalent to single pixel for 800 x 600 monitor
stimParams.dotLifeTime = 2;         %frames

stimParams.diff=[0 5];
stimParams.dotLocatOne=1;   
stimParams.dotCoherenceDiscrimination=1;

%QUEST params
stimParams.QuestTGuess=22.5;
stimParams.QuestTGuessSd=0.1;
stimParams.maxTheta=40;

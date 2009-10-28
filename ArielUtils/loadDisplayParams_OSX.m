function display = loadDisplayParams(varargin)% display = loadDisplayParams([property, value, ...]);%% Created by WAP on 11/3/99%% Routine for loading display params on a given computer.  Uses a default% directory in a default path to load three files: "params", "gamma", and% "spectra".%% Properties that can be changed include any of the required properties contained in the% "displayParams" file, as well as the flag "stereoFlag", which defaults to zero.% In addition, the default path ("path", default depends upon machine type)% and the default display name ("displayName", defaults to "default") can be changed.% The display name determines the subdirectory of "path" that contains the display info.%% The "displayParams" file should contain the following variables (examples in parens):%%  	required:%%		frameRate	 - Refresh rate in Hz (200/3)%		numPixels	 - Number of pixels in display ([640 480])%		dimensions	 - Dimensions of screen in cm ([39 29])%		distance	 - From subject in cm (45)%		cmapDepth	 - Number of DAC bits (10)%		screenNumber - Separate screen output? (0 or 1)%%	descriptive (recommended):%%		computerName	- Name of computer ('Burgundy')%		monitor			- Name of monitor ('ViewSonic')%		card			- Name of card ('Radius10-bit')%		position		- Name of location ('Room 485')%% The "gamma" file must contain the variable gammaTable, which should be% 3 columns (one for each gun) by 256 or 1024 rows (for 8 bit and 10 bit cards,% respectively).%% The "spectra" file must contain the varaible monitorSpectra, which should be% 4 columns (one for each gun and one for white) by 361 rows.  The rows range from% 370 nanometers to 730 nanomenters.AssertOpenGL;if mod(nargin,2)	error('Number of arguments must be even (propertyName, propertyValue, ...).');endhomedir = pwd;% Check to see if monitor path or directory is specifiedfor argNum = 1:(nargin/2)	if strcmp(varargin{2*argNum-1},'path')		displayPath = varargin{2*argNum};	elseif strcmp(varargin{2*argNum-1},'displayName')		displayDir = varargin{2*argNum};	end	endif ~exist('displayPath','var')	displayPath = [pwd filesep 'Displays' filesep];endif ~exist(displayPath,'dir')	error(['Display path ' displayPath ' does not exist and must be created.']);endchdir(displayPath);if ~exist('displayDir','var')	displayDir = 'default';endif ~exist(displayDir,'dir')	error([monitorPath monitorDir ' does not exist and must be created.']);endchdir(displayDir);display = feval('displayParams');if ~isfield(display,'computerName')	display.computerName = 'unspecified';endif ~isfield(display,'monitor')	display.monitor = 'unspecified';endif ~isfield(display,'card')	display.card = 'unspecified';endif ~isfield(display,'position')	display.position = 'unspecified';enddisp(['Initializing ' display.computerName ' computer with ' display.monitor ' display.']);display.stereoFlag = 0;% Adjust any parameters as per special requestsfor argNum = 1:(nargin/2)	propertyName = varargin{2*argNum-1};	propertyVal = varargin{2*argNum};	switch propertyName		case 'frameRate', 		display.frameRate = propertyVal;		case 'numPixels', 		display.numPixels = propertyVal;		case 'dimensions', 		display.dimensions = propertyVal;		case 'distance', 		display.distance = propertyVal;		case 'cmapDepth',		display.cmapDepth = propertyVal;		case 'screenNumber', 	display.screenNumber = propertyVal;		case 'stereoFlag', 		display.stereoFlag = propertyVal;		case 'expt',			display.experiment = propertyVal;			otherwise,			if ~(strcmp(propertyName,'path') | strcmp(propertyName,'displayName'))				error(['Unknown propertyName: ' propertyName]);			end	endenddisplay.pixelSize = mean(display.dimensions./display.numPixels);%display.pixelDepth = display.cmapDepth;	% For backwards compatibilitydisplay.maxGunVal = 2^display.cmapDepth-1;display.numColors = 256;display.reservedColor(1).name='background';%display.reservedColor(1).fbVal = 128; %retinotopic mappingdisplay.reservedColor(1).fbVal = 0; %SNR%display.reservedColor(1).gunVal = [1 1 1]*ceil(display.maxGunVal/2);display.reservedColor(1).gunVal = [130 130 130];% 131 because the first five values of the LUT are taken by reseved colors% This leaves values 5-255, and the mean of these is 130display.reservedColor(2).name='black';display.reservedColor(2).fbVal = 1;display.reservedColor(2).gunVal = [0,0,0];display.reservedColor(3).name='white';display.reservedColor(3).fbVal = 2;display.reservedColor(3).gunVal = [1 1 1]*display.maxGunVal;display.reservedColor(4).name = 'red';display.reservedColor(4).fbVal = 3;display.reservedColor(4).gunVal = [display.maxGunVal 0 0];display.reservedColor(5).name = 'green';display.reservedColor(5).fbVal = 4;display.reservedColor(5).gunVal = [0 display.maxGunVal 0];	% This is also for backwards compatability.  You can ignore these.%display.stepDac = 2^(display.pixelDepth)/display.numColors;  %1 for 8-bit, 4 for 10-bit cards%display.midDac = 2^(display.pixelDepth-1);                   %128 for 8-bit, 512 for 10-bit cards% Load gamma tableif exist([displayPath displayDir '/gamma.mat'],'file')	load gamma	if ~exist('gammaTable','var')		if exist('gamma10','var')			gammaTable = gamma10 * 1023;		else			error('gammaTable not found in gamma.mat');		end	end	display.gammaTable = gammaTable;	fprintf('Loaded gamma table... ');	%if ~exist('invGammaTable','var')%		if exist('invGamma10','var')%			display.invGammaTable = invGamma10;%		else%			display.invGammaTable = [];%			disp('NOTE: invGammaTable not found in gamma.mat');%		end%	else%		display.invGammaTable = invGammaTable;%	endelse	gammaCol = (0:(2^display.cmapDepth-1))';    gammaCol = gammaCol / (2^display.cmapDepth-1);    display.gammaTable = [gammaCol gammaCol gammaCol];	disp('Gamma table not found.  Using linear gamma table instead.');end% Load spectraif exist([displayPath displayDir '/spectra.mat'])	load spectra	if ~exist('monitorSpectra')		disp('Spectra not in spectra file (only important for LMS-specified stimuli)');	else		display.spectra = monitorSpectra(:,1:3);		disp('Loaded spectra.');	endelse	disp('Spectra table not found (only important for LMS-specified stimuli)');endchdir(homedir);% Ugly hack for backwards compatibilitydisplay.resolution = [num2str(display.numPixels(1)) 'x' num2str(display.numPixels(2)) ', ' num2str(display.frameRate) 'Hz'];
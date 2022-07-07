// Instances
var CompassFrame:MovieClip;
var CompassTemperatureHolderInstance:MovieClip;
var CompassMask_mc:MovieClip;
var DirectionRect:MovieClip;
var QuestTitle:MovieClip;
var QuestObjective:MovieClip;

// References
var MarkerList:Array;
var QuestTitleText:TextField;
var QuestTitleEndPieces:MovieClip;
var QuestDescriptionText:TextField;
var QuestObjectiveText:TextField;

function Compass(a_x:Number, a_y:Number, a_hadTemperatureMeter:Boolean):Void
{
	_x = a_x;
	_y = a_y;

	if (a_hadTemperatureMeter)
	{
		_root.HUDMovieBaseInstance.TemperatureMeter_mc = CompassTemperatureHolderInstance;
	}
	else
	{
		// We have it, but we will not use it
		CompassTemperatureHolderInstance.gotoAndStop("Empty");
	}

	_root.HUDMovieBaseInstance.CompassRect = DirectionRect;

	MarkerList = _root.HUDMovieBaseInstance.CompassMarkerList;

	QuestTitleText = QuestTitle.TitleText;
	QuestTitleEndPieces = QuestTitle.EndPieces;

	QuestObjectiveText = QuestObjective.RolloverText;
}

function SetQuestInfo(a_questType:Number, a_questName:String, a_questObjective:String, a_markerIndex:Number):Void
{
	var marker = MarkerList[a_markerIndex];
	marker.movie._xscale = 170;
	marker.movie._yscale = 170;

	QuestTitle._alpha = 100;
	QuestTitleEndPieces.gotoAndStop(a_questType);
	QuestTitleText.text = a_questName.toUpperCase();
	QuestObjective._alpha = 100;
	QuestObjectiveText.text = a_questObjective;

	var bracketsOpenFrame:Number = Math.round(a_questObjective.length * 2.25);
	QuestObjective.gotoAndStop(bracketsOpenFrame);
}

function SetLocationInfo(a_locationName:String, a_markerIndex:Number):Void
{
	var marker:MovieClip = MarkerList[a_markerIndex];
	marker.movie._xscale = 170;
	marker.movie._yscale = 170;
}

function ClearQuestInfos():Void
{
	QuestTitle._alpha = 0;
	QuestObjective._alpha = 0;
}

// Instances
var CompassFrame:MovieClip;
var CompassTemperatureHolderInstance:MovieClip;
var CompassMask_mc:MovieClip;
var DirectionRect:MovieClip;
var QuestTitle:MovieClip;
var QuestDescription:MovieClip;

// References
var MarkerList:Array;
var QuestTitleText:TextField;
var QuestTitleEndPieces:MovieClip;
var QuestDescriptionText:TextField;
var QuestDescription

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
}

function SetQuestInfo(a_title:String, a_endPiecesFrame:Number, a_show:Boolean):Void
{
	if (a_show)
	{
		QuestTitleEndPieces._alpha = 60;
		QuestTitleEndPieces.gotoAndStop(a_endPiecesFrame);
		QuestTitleText.text = a_title.toUpperCase();
	}
	else
	{
		QuestTitleEndPieces._alpha = 0;
		QuestTitleText.text = " ";
	}
}

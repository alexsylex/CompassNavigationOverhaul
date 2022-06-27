var CompassFrame:MovieClip;
var CompassTemperatureHolderInstance:MovieClip;
var CompassMask_mc:MovieClip;
var DirectionRect:MovieClip;
var QuestTitleText:TextField;
var QuestTitleEndPieces:MovieClip;
var CompassMarkerList:Array;

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
		CompassTemperatureHolderInstance.gotoAndStop("Empty");
	}

	_root.HUDMovieBaseInstance.CompassRect = DirectionRect;

	CompassMarkerList = _root.HUDMovieBaseInstance.CompassMarkerList;
}

function SetQuestInfo(a_title:String, a_endPiecesFrame:Number, a_visible:Boolean):Void
{
	QuestTitleEndPieces._visible = a_visible;
	QuestTitleText._visible = a_visible;
	QuestTitleEndPieces.gotoAndStop(a_endPiecesFrame);

	if (a_endPiecesFrame)
	{
		QuestTitleText.text = a_title.toUpperCase();
	}
	else
	{
		QuestTitleText.text = " ";
	}
}

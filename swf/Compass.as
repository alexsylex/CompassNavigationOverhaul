// Instances
var CompassFrame:MovieClip;
var CompassTemperatureHolderInstance:MovieClip;
var CompassMask_mc:MovieClip;
var DirectionRect:MovieClip;
var QuestTitle:MovieClip;
var QuestObjective:MovieClip;
var FocusedMarkerInfo:MovieClip;

// References
var MarkerList:Array;
var FocusedMarker:MovieClip;
var QuestTitleText:TextField;
var QuestTitleEndPieces:MovieClip;
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

	// References
	MarkerList = _root.HUDMovieBaseInstance.CompassMarkerList;

	QuestTitleText = QuestTitle.TitleText;
	QuestTitleEndPieces = QuestTitle.EndPieces;

	QuestObjectiveText = QuestObjective.RolloverText;
}

function SetQuestInfo(a_questType:Number, a_questName:String, a_questObjective:String,
					  a_distance:Number):Void
{
	FocusedMarkerInfo.Distance.TextFieldInstance.text = String(Math.floor(a_distance)) + " m";
	FocusedMarkerInfo.Objective.TextFieldInstance.text = a_questObjective;

	//QuestTitle._alpha = 100;
	//QuestTitleEndPieces.gotoAndStop(a_questType);
	//QuestTitleText.text = a_questName.toUpperCase();
	//QuestObjective._alpha = 100;
	//QuestObjectiveText.text = a_questObjective;

	//var bracketsOpenFrame:Number = Math.round(a_questObjective.length * 2.25);
	//QuestObjective.gotoAndStop(bracketsOpenFrame);
}

function SetLocationInfo(a_locationName:String, a_distance:Number):Void
{
	FocusedMarkerInfo.Distance.TextFieldInstance.text = String(Math.floor(a_distance)) + " m";
	FocusedMarkerInfo.Objective.TextFieldInstance.text = a_locationName;
}

function FocusMarker(a_markerIndex:Number):Void
{
	if (focusedMarker == undefined)
	{
		FocusedMarkerInfo.gotoAndPlay("fadeIn");
	}
	focusedMarker = MarkerList[a_markerIndex].movie;
}

function Update():Void
{
	var MarkerList_length:Number = MarkerList.length;
	for (var i:Number = 0; i < MarkerList_length; i++)
	{
		if (i != a_markerIndex)
		{
			var marker:MovieClip = MarkerList[i].movie;
			marker._xscale /= 1.2;
			marker._yscale /= 1.2;
		}
	}

	if (focusedMarker != undefined)
	{
		FocusedMarkerInfo._alpha = Math.max(focusedMarker._alpha, 75);
		FocusedMarkerInfo._x = localToLocal(focusedMarker, this).x;
	}
}

function UnfocusMarker():Void
{
	if (focusedMarker != undefined)
	{
		focusedMarker = undefined;
		a_markerIndex = -1;

		QuestTitle._alpha = 0;
		QuestObjective._alpha = 0;
		FocusedMarkerInfo.gotoAndPlay("fadeOut");
	}
}

function localToLocal(from:MovieClip, to:MovieClip):Object
{
	var point:Object = { x:0, y:0 };
	from.localToGlobal(point);
	to.globalToLocal(point);
	return point;
}

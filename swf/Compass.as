import com.greensock.TimelineLite;
import com.greensock.TweenLite;
import com.greensock.easing.*;

// Instances
var CompassFrame:MovieClip;
var CompassTemperatureHolderInstance:MovieClip;
var CompassMask_mc:MovieClip;
var DirectionRect:MovieClip;
var QuestTitle:MovieClip;
var QuestObjective:MovieClip;
var DistanceTextFieldInstance:MovieClip;
var ObjectiveTextFieldInstance:MovieClip;

// References
var MarkerList:Array;
var QuestTitleText:TextField;
var QuestTitleEndPieces:MovieClip;
var QuestDescriptionText:TextField;
var QuestObjectiveText:TextField;

var focusedMarkerTween:TweenLite;
var distanceTween: TweenLite;
var objectiveTween: TweenLite;


//
//var focusedMarker:FocusedMarker;


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

	focusedMarkerTween = null;
}

function SetQuestInfo(a_questType:Number, a_questName:String, a_questObjective:String,
					  a_distance:Number):Void
{
	DistanceTextFieldInstance.TextFieldInstance.text = String(Math.floor(a_distance)) + " m";
	ObjectiveTextFieldInstance.TextFieldInstance.text = a_questObjective;

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
	DistanceTextFieldInstance.TextFieldInstance.text = String(Math.floor(a_distance)) + " m";
	ObjectiveTextFieldInstance.TextFieldInstance.text = a_locationName;
}

function Update(a_markerIndex:Number):Void
{
	var focusedMarker:MovieClip = MarkerList[a_markerIndex].movie;

	if (focusedMarkerTween == null)
	{
		focusedMarkerTween = TweenLite.to(focusedMarker, 0.5, {_xscale:130, _yscale:130, ease:Sine.easeIn});
	}

	//focusedMarker._xscale = Math.max(focusedMarker._xscale, 130);
	//focusedMarker._yscale = Math.max(focusedMarker._yscale, 130);

	DistanceTextFieldInstance._alpha = Math.max(focusedMarker._alpha, 75);
	DistanceTextFieldInstance._x = localToLocal(focusedMarker, this).x;

	ObjectiveTextFieldInstance._alpha = Math.max(focusedMarker._alpha, 75);
	ObjectiveTextFieldInstance._x = localToLocal(focusedMarker, this).x;
}

function ClearQuestInfos():Void
{
	QuestTitle._alpha = 0;
	QuestObjective._alpha = 0;
	DistanceTextFieldInstance._alpha = 0;
	ObjectiveTextFieldInstance._alpha = 0;

	focusedMarkerTween.kill();
	focusedMarkerTween = null;
}

function localToLocal(from:MovieClip, to:MovieClip):Object
{
	var point:Object = { x:0, y:0 };
	from.localToGlobal(point);
	to.globalToLocal(point);
	return point;
}

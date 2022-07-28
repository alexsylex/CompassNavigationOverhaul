// Instances
var CompassFrame:MovieClip;
var CompassTemperatureHolderInstance:MovieClip;
var CompassMask_mc:MovieClip;
var DirectionRect:MovieClip;

var FocusedMarkerInstance:FocusedMarker;
var QuestTitle:MovieClip;
var QuestObjective:MovieClip;

// References
var MarkerList:Array = _root.HUDMovieBaseInstance.CompassMarkerList;
var QuestTitleText:TextField = QuestTitle.TitleText;
var QuestTitleEndPieces:MovieClip = QuestTitle.EndPieces;
var QuestObjectiveText:TextField = QuestObjective.RolloverText;

function Compass(a_hadTemperatureMeter:Boolean):Void
{
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
}

function SetMarkerInfo(a_target:String, /*a_location:String, a_objective:String,*/ a_distance:Number, a_heightDifference:Number):Void
{
	FocusedMarkerInstance.SetDistanceAndHeightDifference(a_distance, a_heightDifference);
	FocusedMarkerInstance.Target.TextFieldInstance.text = a_target;
}

function FocusMarker(a_markerIndex:Number):Void
{
	FocusedMarkerInstance.Index = a_markerIndex;

	FocusedMarkerInstance.Movie = MarkerList[FocusedMarkerInstance.Index].movie;
	FocusedMarkerInstance.gotoAndPlay("FadeIn");
	UpdateMarker(FocusedMarkerInstance.Index);
}

function UpdateMarker(a_markerIndex:Number):Void
{
	if (FocusedMarkerInstance.Index != a_markerIndex)
	{
		FocusedMarkerInstance.Index = a_markerIndex;
		FocusedMarkerInstance.Movie = MarkerList[FocusedMarkerInstance.Index].movie;
	}

	FocusedMarkerInstance._x = localToLocal(FocusedMarkerInstance.Movie, this).x;
	FocusedMarkerInstance._alpha = Math.max(FocusedMarkerInstance.Movie._alpha, 75);
	if (_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance._alpha)
	{
		FocusedMarkerInstance.Target.TextFieldInstance._alpha = 0;
	}
	else
	{
		FocusedMarkerInstance.Target.TextFieldInstance._alpha = 100;
	}
}

function UnfocusMarker(a_markerIndex:Number):Void
{
	FocusedMarkerInstance.gotoAndPlay("FadeOut");

	FocusedMarkerInstance.Index = -1;

	QuestTitle._alpha = 0;
	QuestObjective._alpha = 0;
}

function SetMarkersSize():Void
{
	var MarkerList_length:Number = MarkerList.length;
	for (var i:Number = 0; i < MarkerList_length; i++)
	{
		var marker:MovieClip = MarkerList[i].movie;

		if (i == FocusedMarkerInstance.Index)
		{
			marker._xscale = Math.min(130, marker._xscale * 1.15);
			marker._yscale = Math.min(130, marker._yscale * 1.15);
		}
		else
		{
			marker._xscale = Math.min(120, marker._xscale / 1.15);
			marker._yscale = Math.min(120, marker._yscale / 1.15);
		}
	}
}

function localToLocal(from:MovieClip, to:MovieClip):Object
{
	var point:Object = { x:0, y:0 };
	from.localToGlobal(point);
	to.globalToLocal(point);
	return point;
}

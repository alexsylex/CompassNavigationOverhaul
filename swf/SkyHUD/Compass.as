import FocusedMarker;

// Instances
var CompassFrame:MovieClip;
var CompassTemperatureHolderInstance:MovieClip;
var CompassMask_mc:MovieClip;
var DirectionRect:MovieClip;
var CompassFrameAlt:MovieClip;

var FocusedMarkerInstance:FocusedMarker;

// References
var MarkerList:Array = _root.HUDMovieBaseInstance.CompassMarkerList;

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
	_root.HUDMovieBaseInstance.CompassFrame = CompassFrame;
	_root.HUDMovieBaseInstance.CompassFrameAlt = CompassFrameAlt;
	_root.HUDMovieBaseInstance.CompassCard = DirectionRect.CompassDirectionText;
	_root.HUDMovieBaseInstance.CompassCardAlt = DirectionRect.CompassDirectionTextAlt;
}

function SetMarkerInfo(a_target:String, a_distance:Number, a_heightDifference:Number):Void
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

	// iHUD show/hide functionality
	if (DirectionRect._alpha)
	{
		FocusedMarkerInstance._alpha = 100;
	}
	else
	{
		FocusedMarkerInstance._alpha = 0;
	}
}

function UnfocusMarker(a_markerIndex:Number):Void
{
	FocusedMarkerInstance.gotoAndPlay("FadeOut");

	FocusedMarkerInstance.Index = -1;
}

function SetMarkersSize():Void
{
	var MarkerList_length:Number = MarkerList.length;
	for (var i:Number = 0; i < MarkerList_length; i++)
	{
		var marker:MovieClip = MarkerList[i].movie;

		if (i == FocusedMarkerInstance.Index)
		{
			marker._xscale = Math.min(150, marker._xscale * 1.325);
			marker._yscale = Math.min(150, marker._yscale * 1.325);
		}
		else
		{
			marker._xscale = Math.min(135, marker._xscale);
			marker._yscale = Math.min(135, marker._yscale);
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

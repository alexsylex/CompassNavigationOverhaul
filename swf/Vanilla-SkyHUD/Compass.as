#include "../utils.as"

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

function Compass():Void
{
	CompassTemperatureHolderInstance.gotoAndStop("Empty");
	_root.HUDMovieBaseInstance.TemperatureMeter_mc = CompassTemperatureHolderInstance;

	_root.HUDMovieBaseInstance.CompassRect = DirectionRect;

	if (_root.HUDMovieBaseInstance.CompassFrameAlt == undefined)
	{
		CompassFrameAlt._visible = false;
		DirectionRect.CompassDirectionTextAlt._visible = false;
	}
	else
	{
		_root.HUDMovieBaseInstance.CompassFrame = CompassFrame;
		_root.HUDMovieBaseInstance.CompassFrameAlt = CompassFrameAlt;
		_root.HUDMovieBaseInstance.CompassCard = DirectionRect.CompassDirectionText;
		_root.HUDMovieBaseInstance.CompassCardAlt = DirectionRect.CompassDirectionTextAlt;
	}
}

function SetUnits(a_useMetric:Boolean):Void
{
	FocusedMarkerInstance.UseMetricUnits = a_useMetric;
}

function SetMarkerInfo(a_target:String, a_distance:Number, a_heightDifference:Number):Void
{
	FocusedMarkerInstance.SetDistanceAndHeightDifference(a_distance, a_heightDifference);
	FocusedMarkerInstance.Target.TextFieldInstance.text = a_target;
}

function FocusMarker(a_markerIndex:Number):Void
{
	FocusedMarkerInstance.gotoAndPlay("FadeIn");
	UpdateMarker(a_markerIndex);
}

function UpdateMarker(a_markerIndex:Number):Void
{
	FocusedMarkerInstance.Movie = MarkerList[a_markerIndex].movie;
	FocusedMarkerInstance.Index = a_markerIndex;

	// Workaround-fix for pop-up out of compass when changing index
	var focusedMarker_x:Number = localToLocal(FocusedMarkerInstance.Movie, this).x;
	var compassMask_x:Number = localToLocal(CompassMask_mc, this).x;
	if (focusedMarker_x >= compassMask_x)
	{
		FocusedMarkerInstance._x = focusedMarker_x;
	}
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
	FocusedMarkerInstance.Movie = undefined;
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

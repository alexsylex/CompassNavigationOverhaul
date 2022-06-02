var Compass:MovieClip;
var CompassMarkerList:Array;
var ShoutMeterInstance:MovieClip;
var ShoutWarningInstance:MovieClip;
var All:Boolean;
var Favor:Boolean;
var DialogueMode:Boolean;
var StealthMode:Boolean;
var Swimming:Boolean;
var HorseMode:Boolean;
var WarHorseMode:Boolean;

function CompassShoutMeterHolder(a_x:Number, a_y:Number, a_hadTemperatureMeter:Boolean, a_hudElementIndex:Number):Void
{
	_x = a_x;
	_y = a_y;

	delete _parent.ShoutMeter_mc;
	_parent.ShoutMeter_mc = new ShoutMeter(ShoutMeterInstance, ShoutWarningInstance);

	if (a_hadTemperatureMeter)
	{
		_parent.TemperatureMeter_mc = Compass.CompassTemperatureHolderInstance;
	}
	else
	{
		Compass.CompassTemperatureHolderInstance.gotoAndStop("Empty");
	}

	_parent.CompassRect = Compass.DirectionRect;

	_parent.HudElements[a_hudElementIndex] = this;

	All = true;
	Favor = true;
	DialogueMode = true;
	StealthMode = true;
	Swimming = true;
	HorseMode = true;
	WarHorseMode = true;

	CompassMarkerList = _parent.CompassMarkerList;
}

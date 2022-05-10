class CompassShoutMeterBase extends MovieClip
{
	var Compass:MovieClip;
	var ShoutMeterInstance:MovieClip;
	var ShoutWarningInstance:MovieClip;
	var All:Boolean;
	var Favor:Boolean;
	var DialogueMode:Boolean;
	var StealthMode:Boolean;
	var Swimming:Boolean;
	var HorseMode:Boolean;
	var WarHorseMode:Boolean;

	public function CompassShoutMeterBase()
	{
		var hud = _root.HUDMovieBaseInstance;

		hud.CompassShoutMeterHolder.swapDepths(1);
		hud.CompassShoutMeterHolder.removeMovieClip();
		hud.CompassShoutMeterHolder = this;

		delete hud.ShoutMeter_mc;
		hud.ShoutMeter_mc = new ShoutMeter(ShoutMeterInstance, ShoutWarningInstance);
		hud.CompassRect = Compass.DirectionRect;

		hud.HudElements[11] = hud.CompassShoutMeterHolder;

		//hud.InitCompass();
		//hud.CompassShoutMeterHolder.Compass.gotoAndStop("ThreeSixty");
		//hud.CompassThreeSixtyX = hud.CompassRect._x;
		//hud.CompassShoutMeterHolder.Compass.gotoAndStop("Zero");
		//hud.CompassZeroX = hud.CompassRect._x;
		//var CompassMarkerTemp: MovieClip = hud.CompassRect.attachMovie("Compass Marker 2", "temp", hud.CompassRect.getNextHighestDepth());
		//CompassMarkerTemp.gotoAndStop("Quest");
		//hud.CompassMarkerQuest = CompassMarkerTemp._currentframe == undefined ? 0 : CompassMarkerTemp._currentframe;
		//CompassMarkerTemp.gotoAndStop("QuestDoor");
		//hud.CompassMarkerQuestDoor = CompassMarkerTemp._currentframe == undefined ? 0 : CompassMarkerTemp._currentframe;
		//CompassMarkerTemp.gotoAndStop("PlayerSet");
		//hud.CompassMarkerPlayerSet = CompassMarkerTemp._currentframe == undefined ? 0 : CompassMarkerTemp._currentframe;
		//CompassMarkerTemp.gotoAndStop("Enemy");
		//hud.CompassMarkerEnemy = CompassMarkerTemp._currentframe == undefined ? 0 : CompassMarkerTemp._currentframe;
		//CompassMarkerTemp.gotoAndStop("LocationMarkers");
		//hud.CompassMarkerLocations = CompassMarkerTemp._currentframe == undefined ? 0 : CompassMarkerTemp._currentframe;
		//CompassMarkerTemp.gotoAndStop("UndiscoveredMarkers");
		//hud.CompassMarkerUndiscovered = CompassMarkerTemp._currentframe == undefined ? 0 : CompassMarkerTemp._currentframe;
		//CompassMarkerTemp.removeMovieClip();

		All = true;
		Favor = true;
		DialogueMode = true;
		StealthMode = true;
		Swimming = true;
		HorseMode = true;
		WarHorseMode = true;
	}
}

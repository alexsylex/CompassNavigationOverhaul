class CompassShoutMeterBase extends MovieClip
{
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

	public function CompassShoutMeterBase()
	{
		var hud:MovieClip = _root.HUDMovieBaseInstance;

		// Move to original position
		var originalPos:Object = { x:0, y:0 };
		hud.CompassShoutMeterHolder.localToGlobal(originalPos);

		_x = originalPos.x;
		_y = originalPos.y;

		// Get state conditions before replacing
		var hadTemperatureMeter:Boolean = hud["TemperatureMeter_mc"] != undefined;

		var hudElementIndex:Number = -1;
		for (var i:Number = 0; i < hud.HudElements.length; i++)
		{
			if (hud.HudElements[i] == hud.CompassShoutMeterHolder)
			{
				hudElementIndex = i;
			}
		}

		// Replace references to the old clip
		// MovieClip.removeMovieClip() does not remove a movie clip assigned
		// to a negative depth value. Movie clips created in the authoring tool
		// are assigned negative depth values by default. To remove a movie clip
		// that is assigned to a negative depth value, first use the MovieClip.swapDepths()
		// method to move the movie clip to a positive depth value.
		// Reference: http://homepage.divms.uiowa.edu/~slonnegr/flash/ActionScript2Reference.pdf#page=917
		hud.CompassShoutMeterHolder.swapDepths(1);
		hud.CompassShoutMeterHolder.removeMovieClip();
		hud.CompassShoutMeterHolder = this;

		delete hud.ShoutMeter_mc;
		hud.ShoutMeter_mc = new ShoutMeter(ShoutMeterInstance, ShoutWarningInstance);

		if (hadTemperatureMeter)
		{
			hud.TemperatureMeter_mc = Compass.CompassTemperatureHolderInstance;
		}
		else
		{
			Compass.CompassTemperatureHolderInstance.gotoAndStop("Empty");
		}

		hud.CompassRect = Compass.DirectionRect;

		hud.HudElements[hudElementIndex] = this;

		// Init our own variables
		All = true;
		Favor = true;
		DialogueMode = true;
		StealthMode = true;
		Swimming = true;
		HorseMode = true;
		WarHorseMode = true;

		CompassMarkerList = hud.CompassMarkerList;
	}

	function addAttachMovieAnywhere():Void
	{
		MovieClip.prototype.attachMovieAnywhere = function(url:String, id:String, name:String, depth:Number, initObject:Object)
		{
			if(depth == undefined) depth = getNextHighestDepth();
			var parent:MovieClip = this;
			var container:MovieClip = this.createEmptyMovieClip(name, depth);
			var mcLoader:MovieClipLoader = new MovieClipLoader();
			var listener:Object = new Object();
			listener.onLoadInit = function (mc)
			{
				mc.attachMovie(id, "mc", mc.getNextHighestDepth());
				parent[name] = parent[name]["mc"];
			}
			mcLoader.addListener(listener);
			mcLoader.loadClip(url, container);
		}
	}

	function attachTestSymbol():Void
	{
		var attachExternalMovie:Function = MovieClip.prototype.attachMovieAnywhere;

		attachExternalMovie("Test.swf", "Test", "testSymbolSwf", getNextHighestDepth());
		//attachMovie("TestSymbol", "testSymbolLocal", getNextHighestDepth());

		delete MovieClip.prototype.attachMovieAnywhere;
	}
}

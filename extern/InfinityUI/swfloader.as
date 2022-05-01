var version:Number = 1;

function loadPatch(a_swfPath:String):Void
{
	createEmptyMovieClip("container", this.getNextHighestDepth());
	container.loadMovie(a_swfPath);
}

function replace(a_pathToMember:String):Void
{
	//_root.HUDMovieBaseInstance.CompassShoutMeterHolder.unloadMovie();
	//_root.HUDMovieBaseInstance.CompassShoutMeterHolder.swapDepths(1);
	//_root.HUDMovieBaseInstance.CompassShoutMeterHolder.removeMovieClip();

	//_root.HUDMovieBaseInstance.CompassShoutMeterHolder2 = container.CompassShoutMeterHolder;

	//_root.HUDMovieBaseInstance.test.unloadMovie();
	_root.HUDMovieBaseInstance.test.swapDepths(1);
	_root.HUDMovieBaseInstance.test.removeMovieClip();
	_root.HUDMovieBaseInstance.test = container.test;

	var test:MovieClip = _root.HUDMovieBaseInstance.test;
	test.gotoAndStop(1);

	test._x = 640;
	test._y = 360;
	test._alpha = 100;

	skse.log.logger.info("Hello from AS!");
}

function testLog():Void
{
	skse.log.logger.info("Hello from AS!");
}

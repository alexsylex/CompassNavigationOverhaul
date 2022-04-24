class logger
{
	public static function info(str:String):Void
	{
		skse.plugins.logger.info(str);
	}

	public static function error(str:String):Void
	{
		skse.plugins.logger.error(str);
	}
}

import skse.log.logger;

class logger
{
	public static function info(str:String):Void
	{
		skse.log.logger.info(str);
	}

	public static function error(str:String):Void
	{
		skse.log.logger.error(str);
	}
}

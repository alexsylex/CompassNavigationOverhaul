class FocusedMarker extends MovieClip
{
	var HeightIndicatorInstance:MovieClip;
	var Distance:MovieClip;
	var Target:MovieClip;
	var Index:Number;
	var Objective:String;
	var Location:String;
	var Movie:MovieClip;

	public function FocusedMarker()
	{
		Distance.TextFieldInstance.autoSize = "center";
		Index = -1;
	}

	public function SetDistanceAndHeightDifference(a_distance:Number, a_heightDifference:Number):Void
	{
		if (1) // Meters
		{
			Distance.TextFieldInstance.text = String(Math.floor(a_distance * 0.01428)) + " m";
		}
		else // Feet
		{
			Distance.TextFieldInstance.text = String(Math.floor(a_distance)) + " ft";
		}

		Distance.HeightIndicatorInstance._x = Distance.TextFieldInstance._x + Distance.TextFieldInstance._width +
											  Distance.HeightIndicatorInstance._width / 3;

		if (a_heightDifference > 840)
		{
			Distance.HeightIndicatorInstance._alpha = Math.max(Movie._alpha, 75);
			Distance.HeightIndicatorInstance.gotoAndStop("Above");
		}
		else if (a_heightDifference < -840)
		{
			Distance.HeightIndicatorInstance._alpha = Math.max(Movie._alpha, 75);
			Distance.HeightIndicatorInstance.gotoAndStop("Below");
		}
		else
		{
			Distance.HeightIndicatorInstance._alpha = 0;
		}
	}
}

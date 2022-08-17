
class QuestItem extends MovieClip
{
	// Instances
	var Title:MovieClip;
	var ObjectiveItemList:Array;

	//var Test:TextField;

	// References
	private var TitleTextField:TextField;

	// Variables
	public var isBeingShown:Boolean;
	public var ageIndex:Number;
	public var mainQuestFrame:Number;
	public var magesGuildQuestFrame:Number;
	public var thievesGuildQuestFrame:Number;
	public var darkBrotherhoodQuestFrame:Number;
	public var companionQuestFrame:Number;
	public var miscQuestFrame:Number;
	public var daedricQuestFrame:Number;
	public var favorQuestFrame:Number;
	public var civilWarQuestFrame:Number;
	public var dlc01QuestFrame:Number;
	public var dlc02QuestFrame:Number;
	private var bracketOpenLongestNameFrame:Number;

	public function QuestItem()
	{
		ObjectiveItemList = new Array();

		TitleTextField = Title.TitleTextField;
		TitleTextField.textAutoSize = "shrink";

		Title.EndPiece.gotoAndStop("Main");
		mainQuestFrame = Title.EndPiece._currentframe;

		Title.EndPiece.gotoAndStop("MagesGuild");
		magesGuildQuestFrame = Title.EndPiece._currentframe;

		Title.EndPiece.gotoAndStop("ThievesGuild");
		thievesGuildQuestFrame = Title.EndPiece._currentframe;

		Title.EndPiece.gotoAndStop("DarkBrotherhood");
		darkBrotherhoodQuestFrame = Title.EndPiece._currentframe;

		Title.EndPiece.gotoAndStop("Companion");
		companionQuestFrame = Title.EndPiece._currentframe;

		Title.EndPiece.gotoAndStop("Misc");
		miscQuestFrame = Title.EndPiece._currentframe;

		Title.EndPiece.gotoAndStop("Daedric");
		daedricQuestFrame = Title.EndPiece._currentframe;

		Title.EndPiece.gotoAndStop("Favor");
		favorQuestFrame = Title.EndPiece._currentframe;

		Title.EndPiece.gotoAndStop("CivilWar");
		civilWarQuestFrame = Title.EndPiece._currentframe;

		Title.EndPiece.gotoAndStop("DLC01");
		dlc01QuestFrame = Title.EndPiece._currentframe;

		Title.EndPiece.gotoAndStop("DLC02");
		dlc02QuestFrame = Title.EndPiece._currentframe;

		Title.Bracket.gotoAndStop("longest name");
		bracketOpenLongestNameFrame = Title.Bracket._currentframe;
	}

	public function SetQuestInfo(a_type:Number, a_title:String, a_isInSameLocation:Boolean, a_objectives:Array, a_ageIndex:Number):Void
	{
		// Set end piece art
		Title.EndPiece.gotoAndStop(a_type);

		// Set quest title as in the journal
		TitleTextField.text = a_title.toUpperCase();

		// Make room for the title
		var bracketOpenFrame:Number = Math.min(bracketOpenLongestNameFrame, Math.ceil(TitleTextField.textWidth * bracketOpenLongestNameFrame / TitleTextField._width));
		Title.Bracket.gotoAndStop(bracketOpenFrame);

		var yOffset = Title.Bracket._y + Title.Bracket._height;
		for (var i:Number = 0; i < a_objectives.length; i++)
		{
			var objectiveItem:MovieClip = attachMovie("ObjectiveItem", "objective" + i, getNextHighestDepth(), { _x:Title._x, _y: yOffset });

			if (a_isInSameLocation)
			{
				objectiveItem.Objective.gotoAndStop("NormalSelected");
			}
			objectiveItem.Objective.textField.text = a_objectives[i];

			ObjectiveItemList.push(objectiveItem);

			yOffset += 1.1 * objectiveItem._height;
		}

		ageIndex = a_ageIndex;
	}

	public function SetSide(a_side:String):Void
	{
		Title.EndPiece.SideArt.gotoAndStop(a_side);
	}

	public function Show():Void
	{
		gotoAndPlay("FadeIn");

		for (var i:Number = 0; i < ObjectiveItemList.length; i++)
		{
			ObjectiveItemList[i].gotoAndPlay("FadeIn");
		}
	}

	public function Remove():Void
	{
		for (var i:Number = 0; i < ObjectiveItemList.length; i++)
		{
			ObjectiveItemList[i].gotoAndPlay("FadeOut");
		}

		gotoAndPlay("FadeOut");
	}
}

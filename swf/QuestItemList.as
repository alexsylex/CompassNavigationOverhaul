var questItem:QuestItem;
var entries:Array;

var All:Boolean;
var Favor:Boolean;
var DialogueMode:Boolean;
var StealthMode:Boolean;
var Swimming:Boolean;
var HorseMode:Boolean;
var WarHorseMode:Boolean;

var positionY0:Number;
var maxHeight:Number;

var SCALE:Number = 65;

function QuestItemList(a_positionY:Number, a_maxHeight:Number):Void
{
	entries = new Array();

	positionY0 = _root._height * a_positionY;

	var point:Object = { x:10, y:positionY0 };
	globalToLocal(point);
	_x = point.x;
	_y = point.y;

	maxHeight = Stage.height * a_maxHeight;

	All = true;
	Favor = true;
	DialogueMode = true;
	StealthMode = true;
	Swimming = true;
	HorseMode = true;
	WarHorseMode = true;
}

function AddToHudElements():Void
{
	_level0.HUDMovieBaseInstance.HudElements.push(this);
}

function AddQuest(a_type:Number, a_title:String, a_isInSameLocation:Boolean, a_objectives:Array, a_ageIndex:Number):Void
{
	questItem = attachMovie("QuestItem", "questItem", getNextHighestDepth(), { _xscale:SCALE, _yscale:SCALE, positionY0:positionY0, maxHeight:maxHeight });

	entries.push(questItem);

	questItem.SetQuestInfo(a_type, a_title, a_isInSameLocation, a_objectives, a_ageIndex);
	questItem.gotoAndStop("IdleHide");
}

function SetQuestSide(a_side:String):Void
{
	questItem.SetSide(a_side);
}

function Update():Void
{
		// iHUD show/hide compatibility
	if (_root.HUDMovieBaseInstance.CompassShoutMeterHolder.Compass.DirectionRect._alpha &&
		// Toggle Compass Hotkey show/hide compatibility
		_root.HUDMovieBaseInstance.CompassShoutMeterHolder._alpha
		)
	{
		if (entries.length > 1)
		{
			entries.sort(ByAgeThenMiscellaneousQuests);

			var yOffset = 0;
			for (var i:Number = 0; i < entries.length; i++)
			{
				questItem = entries[i];

				questItem._y = yOffset;

				var point:Object = { x:0, y:0 };
				questItem.localToGlobal(point);

				if (point.y >= (positionY0 + maxHeight))
				{
					questItem._visible = false;
				}
				else if (questItem.ObjectiveItemList.length > 1)
				{
					for (var j:Number = 1; j < questItem.ObjectiveItemList.length; j++)
					{
						var objectiveItem:MovieClip = questItem.ObjectiveItemList[j];

						var point:Object = { x:0, y:0 };
						objectiveItem.localToGlobal(point);

						if (point.y >= (positionY0 + maxHeight))
						{
							objectiveItem._alpha = 0.0;
						}
					}
				}

				yOffset += questItem._height + 5;
			}
		}

		_alpha = 100;
	}
	else
	{
		_alpha = 0;
	}
}

function ShowQuest():Void
{
	questItem.Show();
}

function RemoveQuest():Void
{
	questItem.Remove();
}

function ShowAllQuests():Void
{
	for (var i:Number = 0; i < entries.length; i++)
	{
		questItem = entries[i];

		if (!questItem.isBeingShown)
		{
			questItem.Show();
		}
	}
}

function RemoveAllQuests():Void
{
	for (var i:Number = 0; i < entries.length; i++)
	{
		questItem = entries[i];
		if (!questItem.isBeingShown)
		{
			questItem._alpha = 0;
		}
		questItem.Remove();
	}

	entries.splice(0, entries.length);
}

function ByAgeThenMiscellaneousQuests(a_questItem1:QuestItem, a_questItem2:QuestItem):Number
{
	if (a_questItem1.Title.EndPiece._currentframe != a_questItem1.miscQuestFrame &&
		a_questItem2.Title.EndPiece._currentframe == a_questItem2.miscQuestFrame)
	{
		return -1;
	}
	else if (a_questItem1.Title.EndPiece._currentframe == a_questItem1.miscQuestFrame &&
			 a_questItem2.Title.EndPiece._currentframe != a_questItem2.miscQuestFrame)
	{
		return 1;
	}
	else if (a_questItem1.ageIndex > a_questItem2.ageIndex)
	{
		return -1;
	}
	else if (a_questItem1.ageIndex < a_questItem2.ageIndex)
	{
		return 1;
	}

	return 0;
}

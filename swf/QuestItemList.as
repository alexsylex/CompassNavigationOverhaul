var questItem:QuestItem;
var entries:Array;

var All:Boolean;
var Favor:Boolean;
var DialogueMode:Boolean;
var StealthMode:Boolean;
var Swimming:Boolean;
var HorseMode:Boolean;
var WarHorseMode:Boolean;

var SCALE:Number = 65;

function QuestItemList():Void
{
	entries = new Array();

	var point:Object = { x:10, y:90 };
	globalToLocal(point);
	_x = point.x;
	_y = point.y;

	_level0.HUDMovieBaseInstance.HudElements.push(this);

	All = true;
	Favor = true;
	DialogueMode = true;
	StealthMode = true;
	Swimming = true;
	HorseMode = true;
	WarHorseMode = true;
}

function AddQuest(a_type:Number, a_title:String, a_objectives:Array, a_ageIndex:Number):Void
{
	questItem = attachMovie("QuestItem", "questItem", getNextHighestDepth(), { _xscale:SCALE, _yscale:SCALE });

	entries.push(questItem);

	questItem.SetQuestInfo(a_type, a_title, a_objectives, a_ageIndex);
}

function SetQuestSide(a_side:String):Void
{
	questItem.SetSide(a_side);
}

function Update():Void
{
	// iHUD show/hide functionality
	if (_root.HUDMovieBaseInstance.CompassShoutMeterHolder.Compass.DirectionRect._alpha)
	{
		if (entries.length > 1)
		{
			entries.sort(ByAgeThenMiscellaneousQuests);

			var yOffset = 0;
			for (var i:Number = 0; i < entries.length; i++)
			{
				questItem = entries[i];

				questItem._y = yOffset;
				//questItem.Test.text = "[" + i + "] / " + entries.length;

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

function RemoveAllQuests():Void
{
	for (var i:Number = 0; i < entries.length; i++)
	{
		questItem = entries[i];
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

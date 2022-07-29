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

function AddQuest(a_type:Number, a_title:String, a_objectives:Array):Void
{
	questItem = attachMovie("QuestItem", "questItem", getNextHighestDepth(), { _xscale:SCALE, _yscale:SCALE });

	var entriesLen:Number = entries.length;
	if (entriesLen)
	{
		var lastQuestItem:QuestItem = entries[entriesLen - 1];
		questItem._y = lastQuestItem._y + lastQuestItem._height + 5;
	}

	entries.push(questItem);

	questItem.SetQuestInfo(a_type, a_title, a_objectives);
}

function SetQuestSide(a_side:String):Void
{
	questItem.SetSide(a_side);
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

There can be network resources
These are requested on first use and then fetched and cached.
These can only be text and are stored as storage
This means that: 
	- There needs to be a custom network resource class
	- Or the existing resource class can interact with the backend



# Client getters:
get_user_info(userID)
	name
	plays
	kills
	first_login
get_user_top_ranks (userID)
	{ levelID, name, creator }
get_user_most_played (userID)
	{ levelID, name, creator }
get_user_recent_rooms (userID)
	{ roomID, name, creator }
get_user_recent_levels (userID)
	{ levelID, name, creator, seed }
get_user_favorites (userID)
	{ levelID, name, creator }
	{ roomID, name, creator }
get_user_submissions (userID)
	{ levelID, name, creator }
	{ roomID, name, creator }

get_submissions
	{ levelID, name, creator, favorites }
get_most_played
	{ levelID, name, creator, favorites }
get_most_favorites
	{ levelID, name, creator, favorites }



get_room_info (roomID)
	name
	creator
	date
	favorites
	playcount
get_level_info (levelID)
	name
	creator
	date
	favorites
	playcount
	finishcount
	state
get_level_leaderboard (levelID, seed)


# Client setters:
claim_user_name(name)

begin_play (levelID, seed, level_hash)
	check level_hash
	room_playcount++
	level_playcount++
	user_plays++
	if approved
		set levels_most_played
		set levels_most_played_weekly
		set levels_most_played_montly
	set user_most_played
	set user_recent_levels
	set user_recent_rooms
	set user_current

end_play (levelID, seed, play_stats, level_hash)
	check user_current and level_hash
	calculate rank
	user_kills++
	if approved
		set user_top_ranks
	level_finishcount++
	set user_current

set_favorite (levelID, seed)
	level_favorites++
	update user_favorites
	set levels_most_favorites
	maybe make level approved? 

remove_favorite (levelID, seed)
	level_favorites--
	update user_favorites
	set levels_most_favorites
	maybe remove approved status? 

submit_room(room_str)

submit_level(level_str)


# Globals
rooms_most_played (leaderboard)
	{ roomID, playcount }
levels_most_played (leaderboard)
	{ levelID_seed, playcount }
levels_most_favorites (leaderboard)
	{ levelID_seed, favorites }
levels_submissions (leaderboard)
	{ levelID, playcount }
levels_most_played_weekly (leaderboard)
	{ levelID_seed, playcount }
levels_most_played_montly (leaderboard)
	{ levelID_seed, playcount }

# Per level seed

level_(id)_ (seed) (leaderboard)
	{ userID, score }

# Per room

room_(id)
	Size
	Objects
	Enemies
	Status
	Date
room_playcount_(id) (metric)
room_favorites_(id) (metric)

# Per level

level_(id)
	Name
	Creator
	Date
	Status
	Hash
	Rooms { roomID }
level_playcount_(id) (metric)
level_finishcount_(id) (metric)
level_favorites_(id) (metric)
level_seeds_(id)
	{ seed, playcount }

# Per user:

user_stats_(user)
	first_login
	last_login
	story_stage
	favorite_rooms { roomID }
	favorite_levels { levelID }
user_current_(user)
	LevelID
	Seed
	Timestamp
user_plays_(user) (metric)
user_kills_(user) (metric)

user_levels_(user) // Levels created by user
	{ levelID, name }

user_rooms_(user) // Rooms created by user
	{ roomID }

user_recent_levels_(user) (leaderboard)
	 { levelID_seed, date }
	 
user_recent_rooms_(user) (leaderboard)
	 { roomID, date, levelID, Seed }

user_most_played_(user) (leaderboard)
	{ levelID_seed, plays }
	
user_top_ranks_(user) (leaderboard)
	{ levelID_seed, rank }



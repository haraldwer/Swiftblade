This is a game with focus on movement. We want enemies that encourage movement. Combat is melee-only, which means that you have to get close to deal damage. It should not be frustrating to get close. This means that enemies with ranged attacks are preferable. Ghostrunner and Doom has some good examples of these. 
# From other games
### Ghostrunner
One hit kill
Slow fire rate
Simple artstyle
Does not move

Shoots bursts
Reloads often
Attack window during reload

Will jump at you
Landing with a shockwave
Attack window after landing
### Doom
Slowly flying towards you
Stops, shrieks
Flies quickly
Explodes on hit

Hover behavior
Shoots at you
Requires multiple hits

Charges towards
Deals damage + knockback
Strike from behind
# Mood
Simple behaviors, simple graphics. Inspired by Devil Daggers. “Arena-combat” inspired by doom with rooms that have to be cleansed. Blood, death, dark magic and skeletons. Simple meshes with focus on graphical effects. 

### Tech
Animations using a few keyposes. Custom animator. Bound to the enemy AI component. 
Limited to floating for now. This decreases the amount of work. 
Maybe the core animation loops could be made in blender? This would make things a lot nicer to work with...
## Types
### Skull
**Purpose** - Popcorn enemy, simple
**Art** - Skull without eyes. Trail of smoke. 
**Movement** - Floating, moving towards the player. Clumsy but fast movement
**Attack** - Stops, looks towards the player and shrieks, flies quickly straight ahead, explodes when close to the player. Only deadly during explosion. 
### Spitter
**Purpose** - Projectile
**Art** - Skull with hands
**Movement** - Gets fairly close to player before attacking. Wiggle there. 
**Attack** - Spits at player 
### Bat
**Purpose** - Limiting enemy
**Art** - Like a bat, but without lower body
**Movement** - Flaps up and down towards player, sporadic movement.
**Attack** - Will wrap its wings around the player when close. This limits the player movement. Player has to wiggle to break loose. Player will die after a duration.
### Eye
**Art** - Floating eye with rotating rings
**Movement** - Floating slowly, slight bobbing.
**Attack** - Seeks eye contact. Will rotate the player towards its gaze more and more. Player has to pull away. If the player looks for long enough, the player will die. 
### Necromancer
**Art** - Skeleton wizard and floating books
**Movement** - Floating slowly, keeps a distance. Will watch out for player. 
**Attack** - Summon other enemies. Reads from the book during the attack. 

  
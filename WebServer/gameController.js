var editor = {};
var game = {};
var gameSocket = undefined;

function load_game() {
  editor = CodeMirror.fromTextArea(document.getElementById("editor"), {tabSize: 2, lineNumbers: true, undoDepth: 1000});
}

function enter_arena() {
  open_websocket();
}

function open_websocket() {
  var botCodeSocket = new Websock();
  botCodeSocket.open("ws://127.0.0.1:5005");
  
  botCodeSocket.on("open", function () {
    console.log("'botCodeSocket' Connected");
    send_bot_code(botCodeSocket);
  });
  
  botCodeSocket.on("message", function () {
    console.log("'botCodeSocket' Received a message");
  });
  
  botCodeSocket.on("close", function (event) {
    botCodeSocket.close();
    console.log("'botCodeSocket' Disconnected with event: " + event);
  });
  
  botCodeSocket.on("error", function (error) {
    console.error("'botCodeSocket' Error: " + error);
  });
}

function send_bot_code(botCodeSocket) {
  var botCode = editor.getValue();
  botCodeSocket.send_string(botCode);
  console.log("'botCodeSocket' Sent:\n" + botCode);
}

function connect_to_the_game() {
  var initialStateReceived = false;

  gameSocket = new Websock();
  gameSocket.open("ws://127.0.0.1:6005");
  
  gameSocket.on("open", function () {
    console.log("'gameSocket' Connected");
  });
  
  gameSocket.on("message", function () {
    if (initialStateReceived) {
      receive_current_state();
      update_bots();
      update_bullets();
      update_explosions();
      update_bots_statistiques();
    }
    else {
      receive_initial_state();
      initialStateReceived = true;
    }
  });
  
  gameSocket.on("close", function (event) {
    gameSocket.close();
    console.log("'gameSocket' Disconnected with event: " + event);
  });
  
  gameSocket.on("error", function (error) {
    console.error("'gameSocket' Error: " + error);
  });
}

function exit_arena() {
  gameSocket.close();
}

function receive_initial_state() {
  game.width = gameSocket.rQshift16();
  game.height = gameSocket.rQshift16();
  game.numWalls = gameSocket.rQshift8();
  game.numBots = gameSocket.rQshift8();
  
  game.walls = [];
  for (i = 0; i < game.numWalls; i++) {
    game.walls[i] = {};
    game.walls[i].x = gameSocket.rQshift16();
    game.walls[i].y = gameSocket.rQshift16();
    game.walls[i].radius = gameSocket.rQshift8();
  }
  
  game.bots = [];
  for (i = 0; i < game.numBots; i++) {
    var botID = gameSocket.rQshift8();
    game.bots[botID] = {};
    game.bots[botID].markedAsDead = false; // Set to 'true' when the bot is dead (after having changed color in Pixi and CSS). See 'update_bots()'.
    game.bots[botID].alive = true;
    game.bots[botID].scanned = false;
    game.bots[botID].life = 10;
    game.bots[botID].kills = 0;
    game.bots[botID].bullets = 10;
    game.bots[botID].scan = {};
    game.bots[botID].color = get_color(botID);
    game.bots[botID].x = gameSocket.rQshift16();
    game.bots[botID].y = gameSocket.rQshift16();
    game.bots[botID].rotation = gameSocket.rQshift16();
    var nameLength = gameSocket.rQshift8();
    game.bots[botID].name = gameSocket.rQshiftStr(nameLength);
  }
  
  game.numExplosions = 0;
  game.numBotExplosions = 0;
  game.explosions = [];
  game.botExplosions = [];
  
  console.log("Initial game state:");
  console.log(game);
  
  create_view();
}

function receive_current_state() {    
  // The bots.
  var buffer = 0;
  for (i = 0; i < game.numBots; i++) {
    // The first 4 bytes.
    buffer = gameSocket.rQshift8();
    if (i == 0) { // The pause bit is merged into the block of the first bot.
      game.paused = (buffer >> 7) == 1;
    }
    var botID = (buffer >> 1) & 0x3F; // 0X3F is 111111.
    game.bots[botID].rotation = (buffer & 1) << 8;
    game.bots[botID].rotation |= gameSocket.rQshift8();
    buffer = gameSocket.rQshift8();
    game.bots[botID].alive = (buffer >> 7) == 1;
    game.bots[botID].scanned = ((buffer >> 6) & 1) == 1;
    game.bots[botID].life = (buffer >> 2) & 0xF; // 0xF is 1111.
    game.bots[botID].kills = (buffer & 0x3) << 4; // 0x3 is 11.
    buffer = gameSocket.rQshift8();
    game.bots[botID].kills |= buffer >> 4;
    game.bots[botID].bullets = buffer & 0xF; // 0xF is 1111.
    
    // The position.
    game.bots[botID].x = gameSocket.rQshift16();
    game.bots[botID].y = gameSocket.rQshift16();
    
    // The scan, if applicable.
    if (game.bots[botID].life > 0 && game.bots[botID].scanned) {
      game.bots[botID].scan.x = gameSocket.rQshift16();
      game.bots[botID].scan.y = gameSocket.rQshift16();
      game.bots[botID].scan.radius = gameSocket.rQshift16();
    }
  }
  
  // The bullets (the bullets are completely overwritten each iteration because their complete state is sent at each iteration).
  game.bullets = [];
  game.numBullets = gameSocket.rQshift8();
  for (i = 0; i < game.numBullets; i++) {
    game.bullets[i] = {};
    game.bullets[i].x = gameSocket.rQshift16();
    game.bullets[i].y = gameSocket.rQshift16();
  }
  
  // The explosions. The client is responsible for managing the lifetime of the explosions.
  var numNewExplosions = gameSocket.rQshift8();
  for (i = 0; i < numNewExplosions; i++) {
    var newExplosion = {};
    newExplosion.x = gameSocket.rQshift16();
    newExplosion.y = gameSocket.rQshift16();
    newExplosion.radius = 1;
    game.explosions.push(newExplosion);
    game.numExplosions++;
  }
  var numNewBotExplosions = gameSocket.rQshift8();
  for (i = 0; i < numNewBotExplosions; i++) {
    var newBotExplosion = {};
    newBotExplosion.x = gameSocket.rQshift16();
    newBotExplosion.y = gameSocket.rQshift16();
    newBotExplosion.radius = 5;
    game.botExplosions.push(newBotExplosion);
    game.numBotExplosions++;
  }
}

function get_color(colorId) {
  switch (colorId) {
    case 0:
      return 0xFF0000;
    case 1:
      return 0x00FF00;
    case 2:
      return 0x0000FF;
    case 3:
      return 0xFFFF00;
    case 4:
      return 0xFF00FF;
    case 5:
      return 0x00FFFF;
    case 6:
      return 0xFF8000;
    case 7:
      return 0x8000FF;
    default:
      console.error("Unknown color: " + colorId);
      return 0xC0C0C0; // Light grey.
  }
}

function degrees_to_radians(degrees) {
  return degrees * Math.PI / 180;
};

function create_view() {
  // Create an new instance of a pixi container.
  var container = new PIXI.Container();

  // Create a renderer instance, and position it into a canvas.
  var renderer = PIXI.autoDetectRenderer(game.width, game.height, {view: document.getElementById("GameCanvas"), antialias: true}); // Antialiasing is applicable for Chrome only.
  renderer.backgroundColor = 0xFFFFFF;

  // Create all the objects.
  create_bots(container);
  create_walls(container);
  create_bullets(container);
  create_explosions(container);

  // Display bots' statistiques.
  var botCaption = document.getElementById('BotCaption')
  // Create the box containing the statistiques.
  for (i = 0; i < game.numBots; i++) {
    var innerHTML = '';
    innerHTML += '<fieldset id="Bot' + i + '">\n';
    innerHTML += '  <legend>' + game.bots[i].name + '</legend>\n';
    innerHTML += '  <dl>\n';
    innerHTML += '    <dt>Life</dt>\n<dd id="' + 'Bot' + i + '-life"' + '>' + game.bots[i].life + '</dd><br />\n';
    innerHTML += '    <dt>Bullets</dt>\n<dd id="' + 'Bot' + i + '-bullets"' + '>' + game.bots[i].bullets + '</dd><br />\n';
    innerHTML += '    <dt>Kills</dt>\n<dd id="' + 'Bot' + i + '-kills"' + '>' + game.bots[i].kills + '</dd>\n';
    innerHTML += '  </dl>\n';
    innerHTML += '</fieldset>\n';
    botCaption.innerHTML += innerHTML;
  }
  
  // Now that the DOM has been changed, get references to the DOM elements which will change. Note that this cannot be done in the same 'for' loop.
  for (i = 0; i < game.numBots; i++) {
    game.bots[i].caption = {};
    game.bots[i].caption.life = document.getElementById('Bot' + i + '-life');
    game.bots[i].caption.bullets = document.getElementById('Bot' + i + '-bullets');
    game.bots[i].caption.kills = document.getElementById('Bot' + i + '-kills');
  }

  // Start animation.
  requestAnimFrame(animate);
  function animate() {
      requestAnimFrame(animate);
      renderer.render(container);
  }
}

function update_bots() {
  for (i = 0; i < game.numBots; i++) {
    // Draw the shape.
    game.bots[i].graphics.position.x = game.bots[i].x;
    game.bots[i].graphics.position.y = game.bots[i].y;
    game.bots[i].graphics.rotation = degrees_to_radians(game.bots[i].rotation);
    if (!game.bots[i].markedAsDead && game.bots[i].life == 0) { // Change the color to light grey when the bot is dead...
      game.bots[i].graphics.tint = 0xC0C0C0; // ...in Pixi...
      document.getElementById('Bot' + i).id = 'BotDead'; // ...and in CSS.
      game.bots[i].markedAsDead = true; // Mark the bot as dead so we do not try to change its color properties again.
    }
    
    // Draw the scan.
    game.bots[i].scan.graphics.clear(); // In any case clear the scan to avoid residual scan when a bot die.
    if (game.bots[i].scanned) {
      game.bots[i].scan.graphics.lineStyle(1, game.bots[i].color);
      game.bots[i].scan.graphics.drawCircle(game.bots[i].scan.x, game.bots[i].scan.y, game.bots[i].scan.radius);
      game.bots[i].scan.graphics.endFill();
    }
  }
}

// This function is dissociated from update_bots() because update_bots() is called when the captions are not yet defined (when receiving the initial state).
function update_bots_statistiques() {
  for (i = 0; i < game.numBots; i++) {
    game.bots[i].caption.life.innerHTML = game.bots[i].life;
    game.bots[i].caption.bullets.innerHTML = game.bots[i].bullets;
    game.bots[i].caption.kills.innerHTML = game.bots[i].kills;
  }
}

function update_bullets() {
  game.bulletGraphics.clear();
  for (i = 0; i < game.numBullets; i++) {
    game.bulletGraphics.lineStyle(2, 0x000000);
    game.bulletGraphics.drawCircle(game.bullets[i].x, game.bullets[i].y, 2.5);
    game.bulletGraphics.endFill();
  }
}

function update_explosions() {
  game.explosionsGraphics.clear();
  for (i = 0; i < game.numExplosions; i++) {
    if (game.explosions[i].radius < 4) { // The radius goes from 1 to 4.
      game.explosionsGraphics.beginFill(0xFFFF00);
      game.explosionsGraphics.lineStyle(game.explosions[i].radius / 4, 0xFF0000);
      game.explosionsGraphics.drawCircle(game.explosions[i].x, game.explosions[i].y, game.explosions[i].radius);
      game.explosionsGraphics.endFill();
      game.explosions[i].radius += 0.4; // Increased the radius (= decrease lifetime).
    }
    else {
      game.explosions.splice(i, 1); // Remove the explosion from the array.
      game.numExplosions--;
    }
  }
  for (i = 0; i < game.numBotExplosions; i++) {
    if (game.botExplosions[i].radius < 8) { // The radius goes from 5 to 8.
      game.explosionsGraphics.beginFill(0xFFFF00);
      game.explosionsGraphics.lineStyle(game.botExplosions[i].radius / 4, 0xFF0000);
      game.explosionsGraphics.drawCircle(game.botExplosions[i].x, game.botExplosions[i].y, game.botExplosions[i].radius);
      game.explosionsGraphics.endFill();
      game.botExplosions[i].radius += 0.4; // Increased the radius (= decrease lifetime).
    }
    else {
      game.botExplosions.splice(i, 1); // Remove the bot explosion from the array.
      game.numBotExplosions--;
    }
  }
}

function create_walls(container) {
  for (i = 0; i < game.numWalls; i++) {
    game.walls[i].graphics = new PIXI.Graphics();
    game.walls[i].graphics.beginFill(0x808080);
    game.walls[i].graphics.drawCircle(game.walls[i].x, game.walls[i].y, game.walls[i].radius);
    game.walls[i].graphics.endFill();
    game.walls[i].graphics.cacheAsBitmap = true;
    container.addChild(game.walls[i].graphics);
  }
}

function create_bots(container) {
  var botLength = 20;
  var botWidth = 2 * botLength / 3.5;
  for (i = 0; i < game.numBots; i++) {
    // Triangle.
    game.bots[i].graphics = new PIXI.Graphics();
    game.bots[i].graphics.beginFill(0xFFFFFF); // The color is set with the tint, as it is easier to change it latter this way.
    game.bots[i].graphics.lineStyle(1, 0xC0C0C0);
    game.bots[i].graphics.moveTo(0, - 2 * botLength / 3);
    game.bots[i].graphics.lineTo(- botWidth / 2, botLength / 3);
    game.bots[i].graphics.lineTo(botWidth / 2, botLength / 3);
    game.bots[i].graphics.endFill();
    game.bots[i].graphics.tint = game.bots[i].color;
    
    // Scan.
    game.bots[i].scan.graphics = new PIXI.Graphics(); // Nothing to draw until the bot scan.
    
    container.addChild(game.bots[i].graphics);
    container.addChild(game.bots[i].scan.graphics);
  }
  
  update_bots();
}

function create_bullets(container) {
  game.bulletGraphics = new PIXI.Graphics();
  container.addChild(game.bulletGraphics);
}

function create_explosions(container) {
  game.explosionsGraphics = new PIXI.Graphics();
  container.addChild(game.explosionsGraphics);
}

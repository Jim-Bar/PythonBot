#include "Controller.h"

Controller::Controller(Model& model, View& view) : m_model(model), m_view(view)
{}

void
Controller::loop()
{
  bool gameFinished(false), paused(true);
  sf::Event event;
  
  while (!gameFinished)
  {
    // Deal with all events.
    while (m_view.get_window().pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
      {
	m_view.get_window().close();
	gameFinished = true;
      }
      else if (event.type == sf::Event::Resized)
	m_view.resize();
      else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
	paused = !paused;
    }
    
    // Draw the scene.
    m_view.draw(m_model.get_circles());
    m_view.draw(m_model.get_dead_bots());
    m_view.draw(m_model.get_bots());
    m_view.draw(m_model.get_bullets());
    m_view.draw(m_model.get_scans());
    if (paused)
      m_view.draw_pause();
    m_view.render();
    
    if (!paused)
    {
      // Manage bullets.
      update_bullets();
    
      // Manage bots.
      update_bots();
    }
  }
}

void
Controller::update_bullets()
{
  Bullet *bullet(0);
  Bot *bot(0);
  sf::Vector2f hitPoint;
  for (unsigned int i(0); i < m_model.get_bullets().size(); i++)
  {    
    // Get a direct pointer for convenience.
    bullet = (Bullet*) m_model.get_bullets()[i];
    
    if (m_model.collides(bullet, hitPoint)) // Collision with circles and world' boundaries.
    {
      m_view.add_explosion(hitPoint);
      m_model.erase_bullet(i);
    }
    else if (m_model.collides(bullet, &bot, hitPoint)) // Collision with bots.
    {
      m_view.add_explosion(hitPoint);
      m_model.inflict_damages(bot);
      if (!bot->is_alive())
      {
	bullet->get_bot_origin()->add_kill();
        m_view.add_bot_explosion(bot->get_position());
      }
      m_model.erase_bullet(i);
    }
    else
      bullet->move(); // Update position.
  }
}

void
Controller::update_bots()
{
  // Loop over all the bots.
  Bot *bot(0);
  sf::Vector2f hitPoint;
  for (int i(0); i < (int) m_model.get_bots().size(); i++) // See bot death below for the cast.
  {
    // Get a direct pointer for convenience.
    bot = (Bot*) m_model.get_bots()[i];    
    
    // Get and manage the request of each bot.
    BotConnector::Request const& request(bot->get_connector().get_request());
    unsigned int circlesCount(0), botsCount(0), bulletsCount(0); // Results for a scan.
    switch (request.type)
    {
      case BotConnector::SKIP: // The bot asked to skip its turn.
	bot->get_connector().answer_request(bot->get_health(), bot->get_bullets_left(), bot->get_num_kills());
	break;
      case BotConnector::MOVE: // The bot asked to move forward.
        bot->move();
        bot->get_connector().answer_request(bot->get_health(), bot->get_bullets_left(), bot->get_num_kills());
        break;
      case BotConnector::ROTATE: // The bot asked to be rotated.
        bot->rotate(request.rotationDegrees);
        bot->get_connector().answer_request(bot->get_health(), bot->get_bullets_left(), bot->get_num_kills());
        break;
      case BotConnector::FIRE: // The bot asked to fire a bullet.
        if (bot->get_bullets_left() > 0)
          m_model.fire_bullet(bot);
        bot->get_connector().answer_request(bot->get_health(), bot->get_bullets_left(), bot->get_num_kills());
        break;
      case BotConnector::SCAN: // The bot asked a scan.
        bot->set_scan(request.scanDistance, request.scanRadius);
        m_model.scan(bot, request.scanCircles, circlesCount, request.scanBots, botsCount, request.scanBullets, bulletsCount);
        bot->get_connector().answer_scan_request(bot->get_health(), bot->get_bullets_left(), bot->get_num_kills(), circlesCount, botsCount, bulletsCount);
        break;
      default: // The bot did not ask for anything.
        break;
    }

    // Manage collisions.
    Bot *botCollision(0);
    if (m_model.collides(bot, &botCollision, hitPoint))
    {
      m_view.add_explosion(hitPoint);
      m_model.inflict_damages(bot);
      if (!bot->is_alive())
      {
	if (botCollision != 0) // If the bot collides with another bot and not the world, add a kill to this other bot.
	  botCollision->add_kill();
        m_view.add_bot_explosion(bot->get_position());
        i--; // If the bot has been removed from the vector (dead), the next bot will be on the current index.
      }
    }
    
    // Load new bullets.
    bot->load_bullet();
  }
}
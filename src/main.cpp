#include <iostream>
#include <optional>
#include <vector>

#include <SFML/Graphics.hpp>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int FPS_LIMIT = 30;

using Point2D = sf::Vector2f;

// TODO: (Part 1) Define a function that samples a cubic Bezier curve at t in [0, 1].
Point2D getPoint(const std::vector<sf::Vector2f>& pts, float t)
{
    return (1.0f - t)*(1.0f - t)*(1.0f - t)*pts[0] + 3*(1.0f - t)*(1.0f - t)*t*pts[1] + 3*(1.0f-t)*t*t*pts[2] + t*t*t*pts[3];
}

// TODO: (Part 2) Define a function that returns the curve's slope at t in [0, 1].
Point2D getSlope(const std::vector<sf::Vector2f>& pts, float t)
{
    return 3*(1.0f - t)*(1.0f - t)*(pts[1] - pts[0]) + 6*(1.0f - t)*t*(pts[2] - pts[1]) + 3*t*t*(pts[3] - pts[2]);
}

// TODO: (Part 1) Store four control points for the curve.
std::vector<sf::Vector2f> control_points =
{
    {100.0f, 600.0f}, {300.0f, 200.0f}, {500.0f, 200.0f}, {700.0f, 600.0f}
};
// TODO: (Part 2) Track animation time for the square moving along the curve.

float square_animation_time = 0.0f;

// TODO: (Part 3) Track the index of the control point being dragged.

int control_pt_drag_index = -1;

void handleInput(sf::Window& window, bool& shouldQuit) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            shouldQuit = true;
        } else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
            sf::Vector2f cursor_pos(static_cast<float>(mouse->position.x), static_cast<float>(mouse->position.y));
            
            //used this as reference: https://www.sfml-dev.org/documentation/3.0.0/namespacesf_1_1Mouse.html
            if (mouse->button == sf::Mouse::Button::Left)
            {   
                float minimum_dist = 15.0f;
                control_pt_drag_index = -1;
                for (int i = 0; i < control_points.size(); i++)
                {
                    float x_diff = control_points[i].x - cursor_pos.x;
                    float y_diff = control_points[i].y - cursor_pos.y;
                    float current_dist = std::sqrt(x_diff*x_diff + y_diff*y_diff);

                    if (current_dist < minimum_dist)
                    {
                        minimum_dist = current_dist;
                        control_pt_drag_index = i;
                    }
                }
            }
            // TODO: (Part 3) On left-click, select the closest control point
            // using mouse->position and start dragging it.
        } else if (const auto* mouse = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mouse->button == sf::Mouse::Button::Left)
            {
                control_pt_drag_index = -1;
            }
            // TODO: (Part 3) On left-button release, stop dragging.
        } else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>()) {
            if (control_pt_drag_index != -1)
            {
                //used this as reference: https://www.geeksforgeeks.org/cpp/static_cast-in-cpp/
                sf::Vector2f cursor_pos(static_cast<float>(mouse->position.x), static_cast<float>(mouse->position.y));
                sf::Vector2f position_diff = cursor_pos - control_points[control_pt_drag_index];
                
                control_points[control_pt_drag_index] = cursor_pos;

                
                if (control_pt_drag_index%3==0)
                {
                    if (control_pt_drag_index- 1>= 0)
                    {
                        control_points[control_pt_drag_index-1] += position_diff;
                    }
                    
                    if (control_pt_drag_index +1<control_points.size())
                    {
                        control_points[control_pt_drag_index +1] += position_diff;
                    }
  
                }
                
                else
                {
                    int anchor_index = (control_pt_drag_index % 3 == 1) ? control_pt_drag_index - 1 : control_pt_drag_index + 1;
                    int opposite_index = (control_pt_drag_index % 3 == 1) ? control_pt_drag_index - 2 : control_pt_drag_index + 2;

                    
                    if (opposite_index >= 0 && opposite_index<control_points.size())
                    {
                        sf::Vector2f direction_vector = control_points[anchor_index] - control_points[control_pt_drag_index];
                        float len = std::sqrt(direction_vector.x*direction_vector.x + direction_vector.y*direction_vector.y);
                        
                        if (len > 0.0001f)
                        {
                            direction_vector.x /= len; direction_vector.y /= len;
                            float x_diff = control_points[opposite_index].x-control_points[anchor_index].x;
                            float y_diff = control_points[opposite_index].y-control_points[anchor_index].y;
                            float dist = std::sqrt(x_diff*x_diff + y_diff*y_diff);
                            control_points[opposite_index] = control_points[anchor_index] + direction_vector*dist;
                        }
                    }
                }
            }
            // TODO: (Part 3) Move the selected control point to mouse->position.
            // TODO: (Part 4) Maintain matching slopes at shared endpoints.
            // When moving point 3, move point 5 without changing its distance
            // from point 4 (point numbers here start at 1).
        } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            // TODO: (Part 4) '+' adds three control points; '-' removes three,
            // keeping at least four points.
            if (key->code == sf::Keyboard::Key::Equal || key->code == sf::Keyboard::Key::Add)
            {
                sf::Vector2f last = control_points.back();
                control_points.push_back({last.x + 50.0f, last.y - 50.0f});
                control_points.push_back({last.x + 100.0f, last.y - 50.0f});
                control_points.push_back({last.x + 150.0f, last.y});
            }
            else if (key->code == sf::Keyboard::Key::Hyphen || key->code == sf::Keyboard::Key::Subtract)
            {
                if (control_points.size() >= 7) {
                    control_points.pop_back();
                    control_points.pop_back();
                    control_points.pop_back();
                }
            }
        }
    }
}

void render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    // ====== ====== ======
    // TODO: (Part 1) Sample GetPoint over t in [0, 1] and connect samples using the line-drawing
    // code from your project. Draw all four control points as circles after drawing the curve.
    // ====== ====== ======
    
    //in case - is hit too many times
    if (control_points.size()<4)
    {
        return;
    }
    for (size_t i=0; i<control_points.size()-3; i+=3)
    {
        
        std::vector<sf::Vector2f> control_point_chunk = {control_points[i], control_points[i+1], control_points[i+2], control_points[i+3]};
        const int num_points = 200;
        sf::VertexArray curve_line(sf::PrimitiveType::LineStrip,num_points);
            
        for (int pt_idx = 0; pt_idx < num_points; pt_idx++)
        {
            float normalized_time = static_cast<float>(pt_idx)/(num_points-1);
            Point2D curve_pixel_pos = getPoint(control_point_chunk, normalized_time);
            curve_line[pt_idx].position = curve_pixel_pos;
            curve_line[pt_idx].color = sf::Color::White;
        }
            
        window.draw(curve_line);

        sf::VertexArray control_handles(sf::PrimitiveType::Lines, 4);
        control_handles[0].position = control_point_chunk[0];
        control_handles[1].position = control_point_chunk[1];
        control_handles[2].position = control_point_chunk[2];
        control_handles[3].position = control_point_chunk[3];
        for (int pt = 0; pt < 4; pt++) {
            control_handles[pt].color = sf::Color::Yellow; 
        }
        window.draw(control_handles);
    }
    
    for (const auto& point:control_points)
    {
        sf::CircleShape control_circle(7.0f);
        control_circle.setOrigin({7.0f, 7.0f});
        control_circle.setPosition(point);
        control_circle.setFillColor(sf::Color::Red);
        window.draw(control_circle);
    }
    // ====== ====== ======
    // TODO: (Part 2) Draw a small square moving repeatedly along the curve.
    // Use GetSlope to orient it to the curve at each time step.
    // ====== ====== ======
    Point2D square_pixel_pos = getPoint(control_points, square_animation_time);
    Point2D square_slope = getSlope(control_points, square_animation_time);
    
    float square_angle_orientation = std::atan2(square_slope.y, square_slope.x);

    sf::RectangleShape moving_square({20.0f, 20.0f});
    moving_square.setOrigin({10.0f, 10.0f});
    moving_square.setPosition(square_pixel_pos);
    moving_square.setRotation(sf::radians(square_angle_orientation));
    moving_square.setFillColor(sf::Color::Green);
    
    window.draw(moving_square);
    // ====== ====== ======
    // TODO: (Part 3) Draw control handles from point 1 to 2 and point 3 to 4.
    
    // TODO: (Part 4) Draw all connected cubic Bezier segments and their handles.
    // ====== ====== ======

    // ====== ====== ======
    // TODO: (Bonus) Support multiple curves, a Galaga screen overlay at a 1:2 ratio, and exporting
    // curve points as C++ code for Project 1b.
    // ====== ====== ======

    window.display();
}

int main() {
    sf::RenderWindow window;

    try {
        // Initialize window
        window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Bezier Curve Editor");
        window.setFramerateLimit(FPS_LIMIT);
        // Prevent key repeats.
        window.setKeyRepeatEnabled(false);

        bool shouldQuit = false;
        // Main game loop
        while (window.isOpen()) {

            //takes 3 seconds
            square_animation_time += 1.0f/(FPS_LIMIT*3.0f);
            if (square_animation_time > 1.0f)
                square_animation_time = 0.0f;
            handleInput(window, shouldQuit);
            if (shouldQuit) {
                break;
            }
            render(window);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

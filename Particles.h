#ifndef PARTICLES_H
#define PARTICLES_H

#include <SFML/Graphics.hpp>

class ParticleSystem : public sf::Drawable, public sf::Transformable
{
public:

    ParticleSystem() :
        m_particles(1000),
        m_vertices(sf::Points, 1000),
        m_lifetime(sf::seconds(1.f)),
        m_emitter(0.f, 0.f)
    {
    }

    void setShader(sf::Shader& shader) {
        m_shader = &shader;
    }

    void setEmitter(sf::Vector2f position)
    {
        m_emitter = position;
    }

    void update(sf::Time elapsed)
    {
        for (std::size_t i = 0; i < m_particles.size(); ++i)
        {
            // update the particle lifetime
            Particle& p = m_particles[i];
            p.lifetime -= elapsed;

            // if the particle is dead, respawn it
            if (p.lifetime <= sf::Time::Zero)
                resetParticle(i);

            // update the position of the corresponding vertex
            m_vertices[i].position += p.velocity * elapsed.asSeconds();

            // update the alpha (transparency) of the particle according to its lifetime
            float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
            m_vertices[i].color.a = static_cast<sf::Uint8>(ratio * 255);
        }
    }

    void resetAllParticles()
    {
        m_particles.clear();
        m_vertices.clear();

        m_particles.resize(1000);
        m_vertices.resize(1000);

        for (std::size_t i = 0; i < m_particles.size(); ++i)
        {
            resetParticle(i);
        }
    }


private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.shader = m_shader;
        states.transform *= getTransform();
        states.texture = NULL;
        target.draw(m_vertices, states);
        states.shader = nullptr;
    }

private:

    struct Particle
    {
        sf::Vector2f velocity;
        sf::Time lifetime;
    };

    void resetParticle(std::size_t index)
    {
        // give a random velocity and lifetime to the particle
        float angle = (std::rand() % 360) * 3.14f / 180.f;
        float speed = (std::rand() % 20);
        m_particles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
        m_particles[index].lifetime = sf::milliseconds((std::rand() % 2000) + 1000);

        // reset the position of the corresponding vertex
        m_vertices[index].position = m_emitter;
    }

    std::vector<Particle> m_particles;
    sf::VertexArray m_vertices;
    sf::Time m_lifetime;
    sf::Vector2f m_emitter;
    sf::Shader* m_shader;
};
#endif //PARTICLES_H
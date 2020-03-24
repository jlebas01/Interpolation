//
// Created by jlebas01 on 21/03/2020.
//

#include <object/Sphere.hpp>

namespace object {

    Sphere::Sphere(c3ga::Mvec<GLfloat> t_sphere, GLuint64 t_longitude, GLuint64 t_latitude, GLfloat t_rayon,
                   glm::vec3 t_center, GLfloat rayor) :
            sphere(std::move(t_sphere)), modified(true), longitude(t_longitude), latitude(t_latitude), rayon(t_rayon),
            center(t_center), rayonMax(rayor) {
        this->p1 = c3ga::point<GLfloat>(rayon, 0.0, 0.0);
        this->p2 = c3ga::point<GLfloat>(-rayon , 0.0, 0.0);
        this->p3 = c3ga::point<GLfloat>(0.0, rayon, 0.0);
        this->p4 = c3ga::point<GLfloat>(0.0, 0.0, rayon + 0.0);
        this->sphere = p1 ^ p2 ^ p3 ^ p4;
        glGenBuffers(1, &this->vbo);
        glGenVertexArrays(1, &this->vao);
    }


    Sphere::~Sphere() {
        glDeleteBuffers(1, &this->vbo);
        glDeleteVertexArrays(1, &this->vao);
    }

    void Sphere::buildVertices() {
        // Equation paramétrique en (r, phi, theta) de la sphère
        // avec r >= 0, -PI / 2 <= theta <= PI / 2, 0 <= phi <= 2PI
        //
        // x(r, phi, theta) = r sin(phi) cos(theta)
        // y(r, phi, theta) = r sin(theta)
        // z(r, phi, theta) = r cos(phi) cos(theta)
        //
        // Discrétisation:
        // dPhi = 2PI / discLat, dTheta = PI / discLong
        //
        // x(r, i, j) = r * sin(i * dPhi) * cos(-PI / 2 + j * dTheta)
        // y(r, i, j) = r * sin(-PI / 2 + j * dTheta)
        // z(r, i, j) = r * cos(i * dPhi) * cos(-PI / 2 + j * dTheta)

        c3ga::Mvec<GLfloat> dual = sphere.dual();
        dual /= dual[c3ga::E0];
        rayon = std::abs(std::sqrt(dual | dual));

        center = {dual[c3ga::E1], dual[c3ga::E2], dual[c3ga::E3]};


        GLfloat rcpLat = 1.f / latitude, rcpLong = 1.f / longitude;
        GLfloat dPhi = 2 * glm::pi<float>() * rcpLat, dTheta = glm::pi<float>() * rcpLong;

        GLfloat r = rayon;
        //std::cout << r << std::endl;
        std::vector<Vertex> data;
        //
        // Construit l'ensemble des vertex
        for (GLuint64 j = 0; j <= longitude; ++j) {
            GLfloat cosTheta = std::cos(-glm::pi<float>() / 2 + j * dTheta);
            GLfloat sinTheta = std::sin(-glm::pi<float>() / 2 + j * dTheta);

            for (GLuint64 i = 0; i <= latitude; ++i) {
                Vertex vertex{};

                vertex.texture.x = i * rcpLat;
                vertex.texture.y = 1.f - j * rcpLong;

                vertex.normal.x = std::sin(i * dPhi) * cosTheta;
                vertex.normal.y = sinTheta;
                vertex.normal.z = std::cos(i * dPhi) * cosTheta;

                vertex.position = r * vertex.normal + center;

                data.push_back(vertex);
            }
        }

        //int size  = latitude * longitude * 6;

        //GLuint64 idx = 0;
        // Construit les vertex finaux en regroupant les données en triangles:
        // Pour une longitude donnée, les deux triangles formant une face sont de la forme:
        // (i, i + 1, i + discLat + 1), (i, i + discLat + 1, i + discLat)
        // avec i sur la bande correspondant à la longitude
        vertices.clear();
        for (GLuint64 j = 0; j < longitude; ++j) {
            GLuint64 offset = j * (latitude + 1);

            for (GLuint64 i = 0; i < latitude; ++i) {
                this->vertices.push_back(data[offset + i]);
                this->vertices.push_back(data[offset + (i + 1)]);
                this->vertices.push_back(data[offset + latitude + 1 + (i + 1)]);
                this->vertices.push_back(data[offset + i]);
                this->vertices.push_back(data[offset + latitude + 1 + (i + 1)]);
                this->vertices.push_back(data[offset + i + latitude + 1]);
            }
        }
    }

    void Sphere::fillBuffer() {
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * this->vertices.size(), this->vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Set the VAO
        glBindVertexArray(this->vao);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
        glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
        glEnableVertexAttribArray(VERTEX_ATTR_TEXTURE);
        glVertexAttribPointer(
                VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                reinterpret_cast<const GLvoid *>(offsetof(Vertex, position))
        );
        glVertexAttribPointer(
                VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                reinterpret_cast<const GLvoid *>(offsetof(Vertex, normal))
        );
        glVertexAttribPointer(
                VERTEX_ATTR_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                reinterpret_cast<const GLvoid *>(offsetof(Vertex, texture))
        );
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    GLuint Sphere::getVao() {
        GLuint VAO = this->vao;
        return VAO;
    }

    const Vertex *Sphere::data() const {
        return this->vertices.data();
    }


    GLsizei Sphere::getVertexCount() const {
        return static_cast<GLsizei>(this->vertices.size());
    }


    void Sphere::update() {
        buildVertices();
        fillBuffer();

        this->modified = false;
    }

    void Sphere::render() const {
        glBindVertexArray(this->vao);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(this->vertices.size()));
        glBindVertexArray(0);
    }

    void Sphere::dilator(GLfloat scale) {
        this->sphere = utility::Utilitor<GLfloat>::dilator(sphere, scale);
    }

    void Sphere::translator(GLfloat x, GLfloat y, GLfloat z){
        this->sphere = utility::Utilitor<GLfloat>::translator(sphere, x, y, z);
    }

    void Sphere::interpolator(Sphere sphere2, GLfloat alpha){
        this->sphere = utility::Utilitor<GLfloat>::interpolator(sphere, sphere2.getMultiVectors(), alpha);
    }

    glm::vec3 Sphere::getCenter(){
        return this->center;
    }

    void Sphere::blade(){
        this->p1 = c3ga::point<GLfloat>(rayon, 0.0, 0.0);
        this->p2 = c3ga::point<GLfloat>(-rayon + 0.0, 0.0, 0.0);
        this->p3 = c3ga::point<GLfloat>(0.0, rayon, 0.0);
        this->p4 = c3ga::point<GLfloat>(0.0, 0.0, rayon);
        this->sphere = p1 ^ p2 ^ p3 ^ p4;
    }

    c3ga::Mvec<GLfloat> Sphere::getMultiVectors(){
        return this->sphere;
    }



    void Sphere::setMaxRayor(GLfloat rayor){
        this->rayonMax = rayor;
    }

    bool Sphere::checkRayor(){
        c3ga::Mvec<GLfloat> dual = sphere.dual();
        dual = !this->sphere;
        dual /= dual[c3ga::E0];
        rayon = std::abs(std::sqrt(dual | dual));
        std::cout << rayonMax << " :: " << rayon << std::endl;
        return rayonMax >= rayon;
    }

}

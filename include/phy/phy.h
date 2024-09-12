/**
 * @todo create geometry and material
 * @todo remove resetMatrix
 * @todo remove glm from deps
 * @todo make it an header only library
 */
#ifndef __BYTENOL_PHYSICS_FOR_CPP_GAMES_ANIMATION_WITH_OPENGL_H__
#define __BYTENOL_PHYSICS_FOR_CPP_GAMES_ANIMATION_WITH_OPENGL_H__

#include <memory>
#include <string>
#include <climits>
#include <map>
#include <vector>
#include <cmath>
#include <numbers>
#include <iostream>

#include "../../deps/glad/include/glad/glad.h"
#include "../../deps/glfw-3.4/include/GLFW/glfw3.h"

// GLM
#include "../../deps/glm/glm/glm.hpp"
#include "../../deps/glm/glm/ext.hpp"


namespace phy {

    class Canvas2d;

    using CanvasElementPtr = std::unique_ptr<Canvas2d>;

    /// @brief create a glfw window and make it the current context
    /// @param w is the width of the window
    /// @param h is the height of the window
    /// @param title is the title of the window
    /// @return a glfw window
    CanvasElementPtr createContext(const int& w, const int& h, const char* title);


    struct Vector {
        float x;
        float y;

        Vector(float _x = 0.0f, float _y = 0.0f);
        float length() const;
        Vector negate() const;
        Vector normalize() const;

        Vector operator+(const Vector& v) const;
        Vector& operator+=(const Vector& v);

        Vector operator-(const Vector& v) const;
        Vector& operator-=(const Vector& v);

        Vector operator*(const float& s) const;
        Vector& operator*=(const float& s);

        float dot(const Vector& v) const;

        static float distance(const Vector& v1, const Vector& v2);
        static float angleBetween(const Vector& v1, const Vector& v2);
    };


    struct Particle {
        float mass = 1.0f;
        float charge = 0.0f;
        Vector pos;
        Vector vel;
    };

    struct Ball: public Particle {
        float radius = 20.0f;
        int fillColor = 0x0000ff;
        
        Ball() = default;
        Ball(Vector pos, float r);

        void draw(CanvasElementPtr& ctx);
    };


    /// @brief Utility struct to manage opengl buffer objects
    struct Buffer {
        unsigned int vao;
        unsigned int vbo;
        unsigned int ibo;
        unsigned int indexLength;
        ~Buffer();
    };

    class Canvas2d {
        friend CanvasElementPtr createContext(const int& w, const int& h, const char* title);

        public:
            Canvas2d() = default;
            const int& getWidth() const;
            const int& getHeight() const;
            void resetMatrix();
            GLFWwindow* getWindow() const;
            void setFillColor(uint hex, int alpha = 1);
            void drawArc(float x, float y, float r, int startAngle = 0.0f, int endAngle = 0.0f, bool isFill = true);
            void drawLine(float x1, float y1, float x2, float y2);
            ~Canvas2d();

        private:
            Canvas2d(const int& w, const int& h);
            unsigned int createShader(GLenum type, const std::string& src);
            bool linkProgram();
            void initCircleBuffer();
            void addMatrix(glm::mat4 m);
            void initUniformLocation();
            
            int width;
            int height;
            unsigned int shaderProgram;
            std::string title;
            GLFWwindow* window = nullptr;
            Buffer circleGeometry, lineGeometry;

            glm::mat4 mIdentity = glm::mat4(1.0f);
            std::vector<glm::mat4> matrices;

            std::map<std::string, int> uniformLocation;
    };

    Vector::Vector(float _x, float _y) {
        x = _x;
        y = _y;
    }

    inline float Vector::length() const
    {
        return std::sqrt(x * x + y * y);
    }

    inline Vector Vector::negate() const
    {
        return Vector(-x, -y);
    }

    inline Vector Vector::normalize() const
    {
        const float l = length();
        if(l == 0.0f) return Vector(0, 0);
        return Vector(x/l, y/l);
    }

    inline Vector Vector::operator+(const Vector &v) const
    {
        return Vector(x + v.x, y + v.y);
    }

    inline Vector &Vector::operator+=(const Vector &v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }


    inline Vector Vector::operator-(const Vector &v) const
    {
        return Vector(x - v.x, y - v.y);
    }

    inline Vector &Vector::operator-=(const Vector &v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    inline Vector Vector::operator*(const float &s) const
    {
        return Vector(x * s, y * s);
    }

    inline Vector &Vector::operator*=(const float &s)
    {
        x *= s;
        y *= s;
        return *this;
    }

    Ball::Ball(Vector pos, float r) 
    {
        this->pos = pos;
        radius = r;
    }


    void Ball::draw(CanvasElementPtr& ctx)
    {
        ctx->setFillColor(fillColor);
        ctx->drawArc(pos.x, pos.y, radius);
    }

 
    Buffer::~Buffer() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ibo);
    }

    inline const int &Canvas2d::getWidth() const
    {
        return width;
    }

    inline const int &Canvas2d::getHeight() const
    {
        return height;
    }


    inline void Canvas2d::resetMatrix() {
        matrices.clear();
        addMatrix(glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f));
    }


    inline GLFWwindow *Canvas2d::getWindow() const
    {
        return window;
    }

    inline void Canvas2d::setFillColor(uint hex, int alpha)
    {
        const int r = (hex >> 16) & 0xff;
        const int g = (hex >> 8) & 0xff;
        const int b = hex & 0xff;
        glUniform4f(uniformLocation["fillColor"], r / 255, g / 255, b / 255, alpha);
    }

    inline void Canvas2d::drawArc(float x, float y, float r, int startAngle, int endAngle, bool isFill)
    {
        addMatrix(glm::translate(mIdentity, glm::vec3(x, y, 0.0f)) * glm::scale(mIdentity, glm::vec3(r, r, 0.0f)));
        glBindVertexArray(circleGeometry.vao);
        glDrawElements(GL_TRIANGLES, circleGeometry.indexLength, GL_UNSIGNED_INT, (void*)(0));
    }

    inline void Canvas2d::drawLine(float x1, float y1, float x2, float y2)
    {
        glUniformMatrix4fv(uniformLocation["matrix"], 1, false, glm::value_ptr(mIdentity));
        float data[] { x1, y1, x2, y2 };
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 4, data);
        glBindVertexArray(lineGeometry.vao);
        glDrawArrays(GL_LINES, 0, 2);
    }

    inline Canvas2d::~Canvas2d()
    {
        glfwDestroyWindow(window);
    }

    Canvas2d::Canvas2d(const int &w, const int &h)
    {
        width = w;
        height = h;
    }

    inline unsigned int Canvas2d::createShader(GLenum type, const std::string &src)
    {
        const char* _src = src.c_str();
        unsigned int shader = glCreateShader(type);
        glShaderSource(shader, 1, &_src, nullptr);
        glCompileShader(shader);

        int status;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if(!status) {
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << (std::string("ShaderError: ") + infoLog) << std::endl;
            glDeleteShader(shader);
            return UINT_MAX;
        }

        return shader;
    }


    inline bool Canvas2d::linkProgram()
    {
        const std::string vertexShaderSource = R"(#version 330 core
        layout (location=0) in vec2 position;

        uniform mat4 projectionMatrix;
        uniform mat4 matrix;

        void main() {
            gl_Position = matrix * vec4(position, 0.0f, 1.0f);
        })";

        const std::string fragmentShaderSource = R"(#version 330 core
        precision highp float;
        uniform vec4 fillColor;

        out vec4 outColor;

        void main() {
            outColor = fillColor;
        })";

        auto vShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
        auto fShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

        if(vShader == UINT_MAX || fShader == UINT_MAX) {
            return false;
        }

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vShader);
        glAttachShader(shaderProgram, fShader);
        glLinkProgram(shaderProgram);

        glDeleteShader(vShader);
        glDeleteShader(fShader);

        int status;
        char infoLog[512];
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
        if(!status) {
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << (std::string("ProgramError: ") + infoLog) << std::endl;
            return false;
        }

        return true;
    }


    inline void Canvas2d::initCircleBuffer()
    {
        std::vector<float> position{ 0.0f, 0.0f };

        int step = 10;
        for(int i = 0; i <= 360; i += step) {
            float a1 = i * std::numbers::pi / 180;
            position.push_back(std::cos(a1));
            position.push_back(std::sin(a1));
        }

        int curr = 1;
        int max = 360/step;
        std::vector<int> indices;

        while (curr <= max)
        {
            indices.push_back(0);
            indices.push_back(curr);
            indices.push_back(curr + 1);
            curr++;
        }

        circleGeometry.indexLength = indices.size();
        
        glGenVertexArrays(1, &circleGeometry.vao);
        glGenBuffers(1, &circleGeometry.vbo);
        glGenBuffers(1, &circleGeometry.ibo);

        glBindVertexArray(circleGeometry.vao);

        glBindBuffer(GL_ARRAY_BUFFER, circleGeometry.vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * position.size(), position.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, (void*)(0));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, circleGeometry.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // @todo line buffer stuff shouldnt be here
        glGenVertexArrays(1, &lineGeometry.vao);
        glGenBuffers(1, &lineGeometry.vbo);
        glGenBuffers(1, &lineGeometry.ibo);

        glBindVertexArray(lineGeometry.vao);

        glBindBuffer(GL_ARRAY_BUFFER, lineGeometry.vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4, nullptr, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, (void*)(0));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }


    inline void Canvas2d::addMatrix(glm::mat4 m) {
        auto t = matrices.empty() ? mIdentity : matrices.back();
        matrices.push_back(t * m);
        glUniformMatrix4fv(uniformLocation["matrix"], 1, false, glm::value_ptr(matrices.back()));
    }


    inline void Canvas2d::initUniformLocation()
    {
        uniformLocation["fillColor"] = glGetUniformLocation(shaderProgram, "fillColor");
        uniformLocation["matrix"] = glGetUniformLocation(shaderProgram, "matrix");
    }

    CanvasElementPtr createContext(const int& w, const int& h, const char* title) 
    {
        if(!glfwInit()) {
            std::cerr << "Unable to initialize glfw" << std::endl;
            return nullptr;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        CanvasElementPtr cnv(new Canvas2d(w, h));
        cnv->window = glfwCreateWindow(w, h, title, nullptr, nullptr);

        if(!cnv->window) {
            std::cerr << "Unable to create a glfw window\nYour device may not support opengl3.3" << std::endl;
            glfwDestroyWindow(cnv->window);
            return nullptr;
        }

        cnv->title = title;
        glfwMakeContextCurrent(cnv->window);

        if(!gladLoadGL()) {
            std::cerr << "Unable to load opengl headers" << std::endl;
            return nullptr;
        }

        if(!cnv->linkProgram()) {
            std::cerr << "Unable to initialize shader" << std::endl;
            return nullptr;
        }

        glViewport(0, 0, w, h);
        glUseProgram(cnv->shaderProgram);
        cnv->initUniformLocation();
        cnv->initCircleBuffer();
        
        return cnv;

    };

}

#endif
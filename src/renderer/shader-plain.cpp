#include <llmr/renderer/shader-plain.hpp>
#include <llmr/shader/shaders.hpp>
#include <llmr/platform/gl.hpp>

using namespace llmr;

PlainShader::PlainShader()
    : Shader(
        shaders[PLAIN_SHADER].vertex,
        shaders[PLAIN_SHADER].fragment
    ) {
    if (!valid) {
        fprintf(stderr, "invalid plain shader\n");
        return;
    }

    a_pos = glGetAttribLocation(program, "a_pos");
    attributes.emplace_front(a_pos);

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
}
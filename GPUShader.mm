/*
 *	Copyright 2011 Michael Fortin
 *
 *	 Licensed under the Apache License, Version 2.0 (the "License");
 *	 you may not use this file except in compliance with the License.
 *	 You may obtain a copy of the License at
 *	 
 *	 http://www.apache.org/licenses/LICENSE-2.0
 *	 
 *	 Unless required by applicable law or agreed to in writing, software
 *	 distributed under the License is distributed on an "AS IS" BASIS,
 *	 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *	 See the License for the specific language governing permissions and
 *	 limitations under the License.
 */

#include "GPUShader.h"

#include <Foundation/Foundation.h>
#include "APError.h"
#include "Smart.h"

namespace GPU
{
	Shader::Shader(const char *in_szFile)
	{
		NSString *s = [NSString stringWithUTF8String:in_szFile];
		
		// Get the files
		NSString *vertPath = [[NSBundle mainBundle] pathForResource:s ofType:@"vsh"];
		NSString *fragPath = [[NSBundle mainBundle] pathForResource:s ofType:@"fsh"];
		
		m_program = glCreateProgram();
		
		if (vertPath == nil || fragPath == nil)
		{
			throw APError("Unable to find %s in bundle", in_szFile);
		}
		
		// Get the contents
		NSString *vertCode = [NSString stringWithContentsOfFile:vertPath encoding:NSUTF8StringEncoding error:nil];
		NSString *fragCode = [NSString stringWithContentsOfFile:fragPath encoding:NSUTF8StringEncoding error:nil];
		
		
		if (vertCode == nil || fragCode == nil)
		{
			throw APError("Unable to load contents of %s", in_szFile);
		}
		
		
		//Compile...
		GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
		
		const char* vertCCode = [vertCode UTF8String];
		const char* fragCCode = [fragCode UTF8String];
		
		glShaderSource(vertShader, 1, &vertCCode, NULL);
		glCompileShader(vertShader);
		
		glShaderSource(fragShader, 1, &fragCCode, NULL);
		glCompileShader(fragShader);
		
		
		//Display any errors
		int logLength = 0;
		glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0)
		{
			Many<char> x(new char[logLength]);
			glGetShaderInfoLog(vertShader, logLength, &logLength, x());
			
			printf("\n\nLog for Vertex Shader %s\n%s\n", in_szFile, x());
		}
		
		glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0)
		{
			Many<char> x(new char[logLength]);
			glGetShaderInfoLog(fragShader, logLength, &logLength, x());
			
			printf("\n\nLog for Vertex Shader %s\n%s\n", in_szFile, x());
		}
		
		
		//Destroy the object upon any error
		int vertStatus=0, fragStatus=0;
		glGetShaderiv(vertShader, GL_COMPILE_STATUS, &vertStatus);
		glGetShaderiv(fragShader, GL_COMPILE_STATUS, &fragStatus);
		
		if (vertStatus == 0 || fragStatus == 0)
		{
			glDeleteShader(vertShader);
			glDeleteShader(fragShader);
			
			throw APError("Failed to compile shader %s", in_szFile);
		}
		
		
		//Attach shaders to program
		glAttachShader(m_program, vertShader);
		glAttachShader(m_program, fragShader);
		
		
		//Link the program
		int status=0;
		glLinkProgram(m_program);
		
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0)
		{
			Many<char> x(new char[logLength]);
			glGetProgramInfoLog(m_program, logLength, &logLength, x());
			
			printf("\n\nLog for linking %s\n%s\n", in_szFile, x());
		}
		
		glGetProgramiv(m_program, GL_LINK_STATUS, &status);
		if (status == 0)
		{
			glDeleteShader(vertShader);
			glDeleteShader(fragShader);
			throw APError("Failed to link shader %s", in_szFile);
		}
		
		
		//Validate the program
		glValidateProgram(m_program);
		
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0)
		{
			Many<char> x(new char[logLength]);
			glGetProgramInfoLog(m_program, logLength, &logLength, x());
			
			printf("\n\nLog for validating %s\n%s\n", in_szFile, x());
		}
		
		glGetProgramiv(m_program, GL_VALIDATE_STATUS, &status);
		if (status == 0)
		{
			throw APError("Failed to validate program %s", in_szFile);
		}
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
	}
	
	
	Parameter Shader::getParameter(const char *in_name) const
	{
		GLint offset = glGetUniformLocation(m_program, in_name);
		
		if (offset == -1)
			throw APError("Unable to find uniform %s", in_name);
		
		return Parameter(offset);
	}
	
	
	Attribute Shader::getAttribute(const char *in_name) const
	{
		GLint offset = glGetAttribLocation(m_program, in_name);
		
		if (offset == -1)
			throw APError("Unable to find attribute %s", in_name);
		
		return Attribute(offset);
	}
	
	
	Shader::~Shader()
	{
		glDeleteProgram(m_program);
	}
	
	
	
	static GLuint g_bound = 0;		//!< Currently bound shader
	
	BindShader::BindShader(Shader *in_program)
	{
		m_prevProgram = g_bound;
		g_bound = in_program->m_program;
		
		if (g_bound != m_prevProgram)
			glUseProgram(g_bound);
	}
	
	void BindShader::rebind(Shader *in_program)
	{
		if (in_program->m_program != g_bound)
		{
			g_bound = in_program->m_program;
			glUseProgram(g_bound);
		}
	}
	
	BindShader::~BindShader()
	{
		if (g_bound != m_prevProgram)
			glUseProgram(m_prevProgram);
		
		g_bound = m_prevProgram;
	}
}

setupSingleInputValidation('#person-name', {
    validateEvent: 'blur',
    validate: (name) => {
        return name.trim().length === 0 && 'Digite seu nome';
    },
});
setupSingleInputValidation('#person-email', {
    validateEvent: 'blur',
    validate: (email) => {
        const emailRegex = /^(?!\.)(?!.*\.\.)([A-Z0-9_'+-\.]*)[A-Z0-9_'+-]@([A-Z0-9][A-Z0-9\-]*\.)+[A-Z]{2,}$/i;
        return !emailRegex.test(email) && 'Digite um e-mail válido';
    },
});
setupSingleInputValidation('#person-phone', {
    validateEvent: 'blur',
    validate: (phone) => {
        const phoneRegex = /^\(\d\d\) \d{5}-\d{4}$/;
        return (!phoneRegex.test(phone) && 'Insira um número no formato (99) 99999-9999');
    },
});
setupSingleInputValidation('#person-birthdate', {
    validateEvent: 'blur',
    validate: (dateString) => {
        if (!dateString) {
            return 'Adicione sua data de nascimento!!!';
        }
        const date = new Date(dateString);
        const now = new Date();
        const age = now.getFullYear() - date.getFullYear();
        if (age < 18) {
            return 'Você é muito novo :(';
        }
        if (age > 25) {
            return 'Você é muito velho :(';
        }
    },
});
setupSingleInputValidation('#person-picture', {
    validateEvent: 'change',
    validate: (pic) => {
        if (!pic) {
            return 'Adicione uma foto';
        }
    },
});
setupSingleInputValidation('#artistas', {
    validateEvent: 'change',
    validate: (artist) => {
        if (!artist) {
            return 'Selecione um artista';
        }
    },
});
const musicalStyleCheckboxes = Array.from(document.querySelectorAll('[name="musicalStyle"]'));
const favoriteAlbumCheckboxes = Array.from(document.querySelectorAll('[name="albunsFavoritos"]'));
const form = musicalStyleCheckboxes[0].form;
// Esse listener deve ser o último!!! 😈😈😈😈
form.addEventListener('submit', (e) => {
    if (!checkboxGroupSelected(musicalStyleCheckboxes)) {
        return alert('Você deve selecionar ao menos um estilo musical');
    }
    if (!checkboxGroupSelected(favoriteAlbumCheckboxes)) {
        return alert('Você deve selecionar ao menos um álbum');
    }
    const hasActiveErrorMessage = form.querySelector('.error-message:not([hidden=""])');
    if (hasActiveErrorMessage)
        return;
    const data = new FormData(form);
    alert('Formulário válido :)\n\n' + prettyPrintFormData(data));
});
function setupSingleInputValidation(inputSelector, opts) {
    const input = document.querySelector(inputSelector);
    const errorMessage = createErrorMessage(input);
    function validateWrapper() {
        const currentValue = input.value;
        const validationResult = opts.validate(currentValue, input);
        const isValid = typeof validationResult !== 'string';
        if (isValid) {
            hideErrorMessage(errorMessage, input);
            return true;
        }
        setErrorMessage(validationResult, errorMessage, input);
        return false;
    }
    input.addEventListener(opts.validateEvent, () => {
        validateWrapper();
    });
    input.form.addEventListener('submit', (e) => {
        e.preventDefault();
        validateWrapper();
    });
}
function createErrorMessage(forElement) {
    const errorMessage = document.createElement('p');
    const errorElementId = forElement.id.replace('#', 'error-');
    errorMessage.setAttribute('class', 'error-message');
    errorMessage.setAttribute('hidden', '');
    errorMessage.setAttribute('id', errorElementId);
    forElement.after(errorMessage);
    return errorMessage;
}
function hideErrorMessage(errorMessage, forElement) {
    errorMessage.setAttribute('hidden', '');
    forElement.setAttribute('aria-invalid', 'false');
    forElement.removeAttribute('aria-describedby');
}
function setErrorMessage(errorMessage, el, elFor) {
    el.removeAttribute('hidden');
    el.textContent = errorMessage;
    elFor.setAttribute('aria-invalid', 'true');
    elFor.setAttribute('aria-describedby', el.id);
}
function checkboxGroupSelected(inputs) {
    return inputs.some((checkbox) => checkbox.checked);
}
function prettyPrintFormData(formData) {
    const entries = [];
    for (const [key, value] of formData.entries()) {
        if (value instanceof File) {
            entries.push(`${key}: [File: ${value.name} (${value.size} bytes)]`);
        }
        else {
            entries.push(`${key}: ${value}`);
        }
    }
    return entries.join('\n');
}

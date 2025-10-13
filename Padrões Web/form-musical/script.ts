setupValidation('#person-name', {
  validateEvent: 'blur',
  validate: (name) => {
    return name.trim().length === 0 && 'Digite seu nome';
  },
});

setupValidation('#person-email', {
  validateEvent: 'blur',
  validate: (email) => {
    const emailRegex =
      /^(?!\.)(?!.*\.\.)([A-Z0-9_'+-\.]*)[A-Z0-9_'+-]@([A-Z0-9][A-Z0-9\-]*\.)+[A-Z]{2,}$/i;

    return !emailRegex.test(email) && 'Digite um e-mail válido';
  },
});

setupValidation('#person-phone', {
  validateEvent: 'blur',
  validate: (phone) => {
    const phoneRegex = /^\(\d\d\) \d{5}-\d{4}$/;
    return (
      !phoneRegex.test(phone) && 'Insira um número no formato (99) 99999-9999'
    );
  },
});

setupValidation('#person-birthdate', {
  validateEvent: 'blur',
  validate: (dateString) => {
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

function setupValidation(
  inputSelector: string,
  opts: {
    validate: (
      value: string,
      input: HTMLInputElement,
    ) => string | boolean | undefined;
    validateEvent: 'change' | 'blur';
  },
) {
  const input = document.querySelector(inputSelector) as HTMLInputElement;
  const errorMessage = document.createElement('p');

  errorMessage.setAttribute('class', 'error-message');
  errorMessage.setAttribute('hidden', '');
  input.after(errorMessage);

  input.addEventListener(opts.validateEvent, () => {
    const currentValue = input.value;
    const validationResult = opts.validate(currentValue, input);

    const isValid = typeof validationResult !== 'string';

    if (isValid) {
      errorMessage.setAttribute('hidden', '');
      input.setAttribute('aria-invalid', 'false');
      return;
    }

    errorMessage.removeAttribute('hidden');
    errorMessage.textContent = validationResult;
    input.setAttribute('aria-invalid', 'true');
  });
}

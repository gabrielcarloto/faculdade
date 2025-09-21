const tableBody = document.querySelector('tbody')!;

const columnsElements = {
  position: getColumnEl('position'),
  artist: getColumnEl('artist'),
  album: getColumnEl('album'),
  year: getColumnEl('year'),
  sales: getColumnEl('sales'),
};

const BUTTON_TITLES = {
  ascending: 'Ordenar em ordem descrescente',
  descending: 'Ordenar em ordem crescente',
  none: 'Ordenar em ordem crescente',
};

const rows = parseTable();

Object.entries(columnsElements).forEach(([key, th]) => {
  const button = th.querySelector('button')!;

  button.addEventListener('click', () => {
    const currentSort = th.getAttribute('aria-sort') ?? 'descending';
    const nextSort = currentSort === 'ascending' ? 'descending' : 'ascending';
    sortTable(key as keyof typeof columnsElements, nextSort === 'ascending');
    th.setAttribute('aria-sort', nextSort);
    button.setAttribute('title', BUTTON_TITLES[nextSort]);

    Object.values(columnsElements).forEach((col) => {
      if (col === th) return;
      col.setAttribute('aria-sort', 'none');
      col.querySelector('button')!.title = BUTTON_TITLES['none'];
    });
  });
});

function getColumnEl(name: string) {
  const el = document.querySelector(
    `th[data-col="${name}"]`,
  ) as HTMLTableCellElement;

  if (!el) throw new Error('Nome errado :)');

  return el;
}

interface RowData {
  position: { data: number; el: HTMLTableCellElement };
  artist: { data: string; el: HTMLTableCellElement };
  album: { data: string; el: HTMLTableCellElement };
  year: { data: number; el: HTMLTableCellElement };
  sales: { data: number; el: HTMLTableCellElement };
  element: HTMLTableRowElement;
}

function parseTable() {
  const rows = tableBody.querySelectorAll('tr')!;

  const rowsData: RowData[] = [];

  for (const row of rows) {
    const columns = row.querySelectorAll('td');

    const data: RowData = {
      element: row,
      position: { data: Number(columns[0].textContent), el: columns[0] },
      artist: { data: String(columns[1].textContent), el: columns[1] },
      album: { data: String(columns[2].textContent), el: columns[2] },
      year: { data: Number(columns[3].textContent), el: columns[3] },
      sales: {
        data: Number(columns[4].textContent!.replace(',', '.')),
        el: columns[4],
      },
    };

    rowsData.push(data);
  }

  return rowsData;
}

function updateRowSpan(rows: RowData[]) {
  const columnsToCheck = ['artist', 'year', 'sales'] satisfies Array<
    keyof RowData
  >;

  for (const column of columnsToCheck) {
    let i = 0;

    while (i < rows.length) {
      const rowInfo = rows[i][column];
      let span = 1;

      for (let j = i + 1; j < rows.length; j++) {
        const nextRowInfo = rows[j][column];
        if (nextRowInfo.data !== rowInfo.data) break;

        span++;
        nextRowInfo.el.setAttribute('hidden', 'true');
      }

      rowInfo.el.rowSpan = span;
      rowInfo.el.removeAttribute('hidden');
      i += span;
    }
  }
}

function sortTable(columnKey: keyof typeof columnsElements, ascending = true) {
  const collator = new Intl.Collator('pt-br');

  rows.sort((a, b) => {
    const aVal = a[columnKey].data;
    const bVal = b[columnKey].data;

    if (typeof aVal === 'string') {
      return collator.compare(aVal, bVal as string) * (ascending ? 1 : -1);
    }

    const bNum = bVal as number;

    return ascending ? aVal - bNum : bNum - aVal;
  });

  tableBody.innerHTML = '';

  rows.forEach((row) => {
    tableBody.appendChild(row.element);
  });

  updateRowSpan(rows);
}
